#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "read_map.h"
#include "run_qr.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

using namespace std;
using namespace cv;

extern pthread_mutex_t qr_mutex;

// 문자열이 숫자인지 확인하는 함수
bool isNumber(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

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
        // printf("[QR THREAD RUNNING] After finishing debugging, delete delay!\n");
        try{
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Error: Unable to capture frame" << std::endl;
                break;
            }
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            vector<Point> points;

            String info;
            String info_trim;

            if(detector.detect(gray, points)){
                info = detector.decode(gray, points); // error 자주 생기면 try catch
                if (isNumber(info)){
                    ClientAction action;
                    int xy=stoi(info);
                    int x = xy / 10;
                    int y = xy % 10;

                    pthread_mutex_lock(&qr_mutex);
                    *cur_x_ptr = x;
                    *cur_y_ptr = y;
                    pthread_mutex_unlock(&qr_mutex);
                    
                    action.row = x;
                    action.col = y;
                    action.action = 1; // (1: set trap, 0: none) -> error
                    send(sock, &action, sizeof(ClientAction), 0);
                    printf("[QR Thread Running & Detect QR] x: %d, y: %d\n", x, y);
    
                }
            }
        }
        catch{
            printf("########################");
        }
    }
    cap.release();
}
