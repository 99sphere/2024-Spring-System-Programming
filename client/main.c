#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "run_qr.hpp"
#include "read_map.h"


pthread_mutex_t map_mutex;
pthread_mutex_t qr_mutex;


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <Server IP> <port number> <cur dir>\n", argv[0]);
        return 1;
    }

    int cur_dir = atoi(argv[3]);
    if (!((cur_dir == 1) || (cur_dir == 3))){
        printf("Current direction must be 1 or 3.\n");
        return 2;
    }

    // line tracing()

    int cur_x = -1;
    int cur_y = -1;
    int set_bomb = 0;
    int sock = 0;
    struct sockaddr_in serv_addr;

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

	const int PORT = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // DGIST 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // ClientAction 구조체 생성 및 설정

    pthread_mutex_init(&qr_mutex, NULL);
    pthread_mutex_init(&map_mutex, NULL);
    // 평생 실행될 qr detection을 thread로 실행
    pthread_t thread_qr;
    qr_thread_data_t qr_thread_data;

    // Init data struct for qr thread
    qr_thread_data.sock = sock;
    qr_thread_data.cur_x_ptr = &cur_x;
    qr_thread_data.cur_y_ptr = &cur_y;
    qr_thread_data.set_bomb_ptr = &set_bomb;

    // Create Thread
    int qr_thread_ret;
    qr_thread_ret = pthread_create(&thread_qr, NULL, run_qr, (void*)&qr_thread_data);
    if (qr_thread_ret) {
        printf("Error: unable to create thread, %d\n", qr_thread_ret);
        exit(-1);
    }

    
    // 평생 실행될 server에서 map을 받아오는 코드 thread로 실행
    // 평생 실행될 qr detection을 thread로 실행
    pthread_t thread_map;
    map_thread_data_t map_thread_data;
    
    DGIST raw_map={0,};
    // 스레드 데이터 초기화
    map_thread_data.sock = sock;
    map_thread_data.raw_map_ptr = &raw_map;

    // 스레드 생성
    int map_thread_ret;
    map_thread_ret = pthread_create(&thread_map, NULL, read_map, (void*)&map_thread_data);
    if (map_thread_ret) {
        printf("Error: unable to create thread, %d\n", map_thread_ret);
        exit(-1);
    }

    int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int prev_x = -1;
    int prev_y = -1;

    int next_dir;
    int next_x;
    int next_y;
    while(1){
        int score_min = -1;

        // printf("[Main Loop] cur_x: %d, cur_y: %d, cur_status: %d", cur_x, cur_y, raw_map.map[cur_x][cur_y].item.status);
        for(int d = 0;d<4;d++){
            int score;
            int diff = d - cur_dir;
            if (diff < 0){
                diff += 4;
            }
            if (diff != 2){
                int nx = cur_x+dir[d][0];
                int ny = cur_y+dir[d][1];
    
                if((ny >= 0 && nx >= 0) && (ny < 5 && nx < 5)){
                    if (raw_map.map[nx][ny].item.status==1 || raw_map.map[nx][ny].item.status==0){
                        if (raw_map.map[nx][ny].item.status==1){
                            score = raw_map.map[nx][ny].item.score;
                        }
                        else{
                            score = 0;
                        }
                        if (score > score_min){
                            next_dir = d;
                            next_x = nx;
                            next_y = ny;
                        }
                    }
                }
            }
        }
        
        if (next_dir==-1){ // trap 또는 빈칸으로 둘러싸인 경우, 지금은 유턴
            // turn_left();
            // turn_left();
            // straight();
            cur_dir -= 2;
            if (cur_dir < 0){
                cur_dir += 4;
            }        
        }

        else{
            int calc_rot = cur_dir - next_dir;
            if (calc_rot < 0){
                calc_rot += 4;
            }

            if (calc_rot == 0){
                // straight();
            } 

            else if (calc_rot == 1){
                // turn_left();
                // straight();
                cur_dir -= 1;
                if (cur_dir < 0){
                    cur_dir += 4;
                }
            }

            else if (calc_rot == 3){
                // turn_right();
                // straight();
                cur_dir += 1;
                if (cur_dir > 4){
                    cur_dir -= 4;
                }
            }

            if ((next_x==1 && next_y==1) || (next_x==1 && next_y==3) || (next_x==3 && next_y==1) || (next_x==3 && next_y==1)){
                set_bomb = 1;
            }
        }
    }
    // 스레드가 종료될 때까지 대기
    pthread_join(thread_qr, NULL);
    pthread_join(thread_map, NULL);

    pthread_mutex_destroy(&qr_mutex);
    pthread_mutex_destroy(&map_mutex);

    close(sock);
    return 0;
}