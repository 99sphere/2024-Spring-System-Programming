#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(){
    
    VideoCapture cap(0);
    if (!cap.isOpened()) {
	std::cerr << "Error: Unable to open the camera" << std::endl;
        return -1;
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
	    if(!info.empty()){
	        cout << "Decoded Data: " << info << endl;
	    }
	    else{
	        cout << "None" << endl;
	    }
	}

        if (cv::waitKey(30) == 'q') {
            break;
        }
    }
 
    cap.release();
    return 0;
}
