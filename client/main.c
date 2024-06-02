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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <port number>\n", argv[0]);
        return 1;
    }

    // line tracing()

    int cur_x = -1;
    int cur_y = -1;

    if (argc != 3) {
        printf("Usage: %s <Server IP> <port number>\n", argv[0]);
        return 1;
    }

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

    // 평생 실행될 qr detection을 thread로 실행
    pthread_t thread_qr;
    qr_thread_data_t qr_thread_data;

    // 스레드 데이터 초기화
    qr_thread_data.sock = sock;
    qr_thread_data.cur_x_ptr = &cur_x;
    qr_thread_data.cur_y_ptr = &cur_y;

    printf("FUCK\n");

    // 스레드 생성
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
    
    DGIST raw_map;
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
    // 경로탐색 알고리즘

    while(1){
        printf("[Main Algorithm]\n");
        printMap(raw_map);
        sleep(1);
    }
    // 스레드가 종료될 때까지 대기
    pthread_join(thread_qr, NULL);
    pthread_join(thread_map, NULL);

    close(sock);
    return 0;
}