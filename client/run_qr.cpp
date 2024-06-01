#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include "run_qr.hpp"
#include <iostream>
#include "client.h"

using namespace std;
using namespace cv;

// thread 돌릴때 인자 전달 어떻게 하는지
void* run_qr(void* arg){
// int run_qr(int sock, int *x_ptr, int *y_ptr){
    qr_thread_data_t* data = (qr_thread_data_t*) arg;
    int sock = data->sock;
    int* x_ptr = data->x_ptr;
    int* y_ptr = data->y_ptr;
    
    VideoCapture cap(0);
    if (!cap.isOpened()) {
	std::cerr << "Error: Unable to open the camera" << std::endl;
        return -1;
    }
    
    cv::QRCodeDetector detector;
    cv::Mat frame, gray;
   
    ClientAction action={0,};

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
            int xy=stoi(info);
            int x = xy / 10;
            int y = xy % 10;
            *x_addr = x;
            *y_addr = y;
            action.row = x; // x 값 설정
            action.col = y; // y 값 설정
            action.action = 0; // 함정 설정 여부 설정 (1: 함정 설정, 0: 함정 설정 안 함)
            send(sock, &action, sizeof(ClientAction), 0);
        }
    }
    cap.release();
    return 0;
}
