#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "read_map.h"
#include <pthread.h>
#include <semaphore.h>


void printMap(DGIST dgist) {
    printf("Map:\n");
    for (int i = 0; i < MAP_ROW; i++) {
        for (int j = 0; j < MAP_COL; j++) {
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
        }
        printf("\n");
    }
}

// thread에 인자를 어떻게 넘기냐
void* read_map(void* arg) {
// void *read_map(int sock, DGIST* raw_map_ptr) {
    map_thread_data_t* data = (map_thread_data_t*) arg;
    int sock = data->sock;
    DGIST* raw_map_ptr = data->raw_map_ptr;

    char buffer[sizeof(DGIST)] = {0};
    int valread;
    while(1){
        // 서버로부터 데이터 받아오기
        if ((valread = read(sock, buffer, sizeof(DGIST))) == 0) {
            printf("Server disconnected\n");
        }
        // 받아온 데이터를 DGIST 구조체로 변환
        memcpy(raw_map_ptr, buffer, sizeof(DGIST));

        // 맵 데이터 출력
        // printMap(received_dgist);
    }
}