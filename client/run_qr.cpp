#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "client.h"
#include "run_qr.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

using namespace std;
using namespace cv;

// thread 돌릴때 인자 전달 어떻게 하는지
void* run_qr(void* arg){
// int run_qr(int sock, int *x_ptr, int *y_ptr){
    qr_thread_data_t* data = (qr_thread_data_t*) arg;
    int sock = data->sock;
    int* cur_x_ptr = data->cur_x_ptr;
    int* cur_y_ptr = data->cur_y_ptr;
    
    VideoCapture cap(0);
    if (!cap.isOpened()) {
	    std::cerr << "Error: Unable to open the camera" << std::endl;
    }
    
    cv::QRCodeDetector detector;
    cv::Mat frame, gray;

    printf("[CALL QR THREAD] \n");
    
    while (true) {  
        printf("[QR THREAD RUNNING]\n");
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Unable to capture frame" << std::endl;
            break;
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Point> points;
        String info = "0";

        if(detector.detect(gray, points)){
            info = detector.decode(gray, points);
            ClientAction action;

            // int xy=stoi(info);
            // int x = xy / 10;
            // int y = xy % 10;
            *cur_x_ptr = 0;
            *cur_y_ptr = 0;
            action.row = 0; // x 값 설정
            action.col = 0; // y 값 설정
            // action.action = 0; // 함정 설정 여부 설정 (1: 함정 설정, 0: 함정 설정 안 함)
            printf("current loc: (%d, %d)", 0, 0);
            send(sock, &action, sizeof(ClientAction), 0);
        }
        sleep(1);
    }
    cap.release();
}
