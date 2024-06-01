#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>


#define MAX_CLIENTS 2
#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW _MAP_ROW + 1
#define MAP_COL _MAP_COL + 1

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <port number>\n", argv[0]);
        return 1;
    }

    // line tracing()

    int cur_x = -1;
    int cur_y = -1;


    // 평생실행될 qr detection을 thread로 실행
    pthread_t thread1, thread2;
    int thread_arg1 = 1;
    int thread_arg2 = 2;


    // 평생실행될 server에서 map을 받아오는 코드 thread로 실행
    pthread_t thread1, thread2;
    int thread_arg1 = 1;
    int thread_arg2 = 2;

    // 경로탐색 알고리즘


    return 0;
}