// 클라이언트 코드
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 2
#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW _MAP_ROW + 1
#define MAP_COL _MAP_COL + 1

typedef struct{
    int socket;
    struct sockaddr_in address;
    int row;
    int col;
    int score;
    int bomb;
} client_info;

enum Status{
    nothing,
    item,
    trap
};

typedef struct{
    enum Status status;
    int score;
} Item;

typedef struct {
    int row;
    int col;
    Item item; 
} Node;

typedef struct{
    client_info players[MAX_CLIENTS];
    Node map[MAP_ROW][MAP_COL];
} DGIST;

// ClientAction 구조체 정의
typedef struct {
    int row;
    int col;
    int action;
} ClientAction;


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



int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <port number>\n", argv[0]);
        return 1;
    }

	const int PORT = atoi(argv[2]);

    int sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    // char *hello = "Hello from client";
    // char buffer[1024] = {0};
    char buffer[sizeof(DGIST)] = {0};

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // // 서버에 연결
    // if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    //     printf("\nInvalid address/ Address not supported \n");
    //     return -1;
    // }

    // DGIST 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // ClientAction 구조체 생성 및 설정

    // 서버에 데이터 전송
    int x=0;
    int y=0;
    int cnt=0;

    while(1){

        // 서버로부터 데이터 받아오기
        if ((valread = read(sock, buffer, sizeof(DGIST))) == 0) {
            printf("Server disconnected\n");
            return -1;
        }

        // 받아온 데이터를 DGIST 구조체로 변환
        DGIST received_dgist;
        memcpy(&received_dgist, buffer, sizeof(DGIST));

        // 맵 데이터 출력
        printMap(received_dgist);

        ClientAction action;
        action.row = x; // x 값 설정
        action.col = y; // y 값 설정
        action.action = 0; // 함정 설정 여부 설정 (1: 함정 설정, 0: 함정 설정 안 함)
        send(sock, &action, sizeof(ClientAction), 0);
        printf("Action sent to server\n");
        if (cnt%2 == 0){
            x += 1;
            x = x % 4;
        }
        else{
            y += 1;
            y = y % 4;
        }
        cnt += 1;

        // 이제 received_dgist를 사용하여 원하는 작업을 수행할 수 있습니다.
        printf("\nPlayers:\n");
        for (int i = 0; i < MAX_CLIENTS; i++) {
            printf("Player %d: Row=%d, Col=%d, Score=%d, Bomb=%d\n",
                i+1, received_dgist.players[i].row, received_dgist.players[i].col,
                received_dgist.players[i].score, received_dgist.players[i].bomb);
    }

    }
    close(sock);

    // // 데이터 송신 및 수신
    // while (1){
    //     send(sock, hello, strlen(hello), 0);
    //     printf("Hello message sent\n");
    //     valread = read(sock, buffer, 1024);
    //     printf("%s\n", buffer);
    // }

    return 0;
}