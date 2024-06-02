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

    int my_dir = atoi(argv[3]);
    if (!((my_dir == 1) || (my_dir == 3))){
        printf("Current direction must be 1 or 3.\n");
        return 2;
    }

    // line tracing()

    int cur_x = -1;
    int cur_y = -1;

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
                  
    int dir[4][2] = {{1,0},{-1,0},{0,-1},{0,1}};
    int prev_x;
    int prev_y;

    while(1){
        // 서버에서 데이터를 받아왔나?
        printf("[Main Loop] cur_x: %d, cur_y: %d, cur_status: %d", cur_x, cur_y, raw_map.map[cur_x][cur_y].item.status);
        // cur_x
        // cur_y
        // for(int d = 0;d<4;d++){
        //     int ny = y+dir[d][0];
        //     int nx = x+dir[d][1];
            
        //     if(( ny >= 0 && nx >= 0) && (ny < 5 && nx < 5)){
        //         raw_map.map[nx][ny].item.status
        //         if(raw_map.map[nx][ny]){

        //         }
        //     }
        // }
        printf("[Main Algorithm]\n");
        printMap(raw_map);
        sleep(1);



        /*
        
        switch (dgist.map[i][j].item.status) {
            case nothing:
                printf("- ");
                break;
            case item:
                printf("%d ", dgist.map[i][j].item.score);
                break;
            case trap:
                printf("T ");
                break;
        }
            
            */
    }
    // 스레드가 종료될 때까지 대기
    pthread_join(thread_qr, NULL);
    pthread_join(thread_map, NULL);

    pthread_mutex_destroy(&qr_mutex);
    pthread_mutex_destroy(&map_mutex);

    close(sock);
    return 0;
}