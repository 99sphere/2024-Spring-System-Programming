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

    while (true) {  
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Unable to capture frame" << std::endl;
            break;
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Point> points;
        if(detector.detect(gray, points)){
            String info = detector.decode(gray, points);
            ClientAction action;
            int xy=stoi(info);
            int x = xy / 10;
            int y = xy % 10;
            *cur_x_ptr = x;
            *cur_y_ptr = y;
            action.row = x; // x 값 설정
            action.col = y; // y 값 설정
            // action.action = 0; // 함정 설정 여부 설정 (1: 함정 설정, 0: 함정 설정 안 함)
            send(sock, &action, sizeof(ClientAction), 0);
        }
    }
    cap.release();
}
