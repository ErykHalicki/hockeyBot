/*
* g++ -I /opt/homebrew/include/opencv4 -std=c++11 tracking.cpp serialib/lib/serialib.cpp -o tracking.out $(pkg-config --cflags --libs opencv4)
*/
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <unistd.h>
#include "serialib/lib/serialib.h"
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;
#define SERIAL_PORT "/dev/cu.usbserial-AQ00XQZC"
void send_data(serialib serial,const char* data){
	cout<<"sent data: "<<data;
    serial.writeString(data);
	waitKey(100);
}

int main(int argc, char **argv) {

	int waitTime=1800;
    serialib serial;
	clock_t start;
	start=clock();
	//add non blocking spacing
    char errorOpening = serial.openDevice(SERIAL_PORT, 9600);
    Mat frame, gray, frameDelta, thresh,thresh2, lastFrame, firstFrame;
	Mat empty(288, 512, CV_8UC3, Scalar(100,100, 100));
    vector<vector<Point> > cnts, cnts2;
    VideoCapture camera(0); //open camera
    
    //set the video size to 512x288 to process faster
    camera.set(3, 512);
    camera.set(4, 288);

    sleep(1);
    camera.read(frame);

	cvtColor(frame, lastFrame, COLOR_BGR2GRAY);
    GaussianBlur(lastFrame, lastFrame, Size(51, 51), 0);
	
	/*if (errorOpening!=1){
			printf("not working");
			return errorOpening;
	}
    printf ("Successful connection to %s\n",SERIAL_PORT);
*/
    //convert to grayscale and set the first frame
	int xMeanMean=0,xMeanFirst=0, cntsNumFirst=0, xMeanSecond=0,cntsNumSecond=0;
	float screenMiddleSize=0.1, motionSizePercentageThresh=0.06;
	Rect temp;
    while(camera.read(frame)) {

        //convert to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(61, 61), 0);
		waitKey(30);
		//for(int i=0;i<10;i++)
		camera.read(firstFrame);
		waitKey(30);
		//for(int i=0;i<10;i++)
		camera.read(lastFrame);

		cvtColor(lastFrame, lastFrame, COLOR_BGR2GRAY);
    	GaussianBlur(lastFrame, lastFrame, Size(61, 61), 0);
		cvtColor(firstFrame, firstFrame, COLOR_BGR2GRAY);
    	GaussianBlur(firstFrame, firstFrame, Size(61, 61), 0);

        //compute difference between first frame and current frame
        absdiff(lastFrame, gray, frameDelta);
        threshold(frameDelta, thresh, 20, 255, THRESH_BINARY);
        absdiff(firstFrame, gray, frameDelta);
        threshold(frameDelta, thresh2, 20, 255, THRESH_BINARY);

        dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
        dilate(thresh2, thresh2, Mat(), Point(-1,-1), 2);

        findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        findContours(thresh2, cnts2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for(int i = 0; i< cnts.size(); i++) {
            if(contourArea(cnts[i]) > 200) {
				cntsNumFirst++;
				drawContours(frame, cnts, i, Scalar(0,255,0), 3);
				temp=boundingRect(cnts[i]);
				rectangle(frame,temp,Scalar(0,255,0));
				xMeanFirst+=temp.x+(temp.width/2);
								//break;
            }
        }
		for(int i = 0; i< cnts2.size(); i++) {
            if(contourArea(cnts2[i]) > 200) {
				cntsNumSecond++;
				drawContours(frame, cnts2, i, Scalar(0,0,255), 3);
				temp=boundingRect(cnts2[i]);
				rectangle(frame,temp,Scalar(0,0,255));
				xMeanSecond+=temp.x+(temp.width/2);
								//break;
            }
        }
		if(cntsNumFirst && cntsNumSecond){
			xMeanFirst/=cntsNumFirst;
			xMeanSecond/=cntsNumSecond;
			xMeanMean=(xMeanFirst+xMeanSecond)/2;

			if(xMeanSecond-xMeanFirst>frame.size().width*0.02){serial.writeString("left\n");cout<<"dynamic left\n";waitKey(waitTime);}
			else if(xMeanSecond-xMeanFirst<-frame.size().width*0.02){serial.writeString("right\n");cout<<"dynamic right\n";waitKey(waitTime);}
			else if(xMeanMean>frame.size().width/2 + frame.size().width*0.2){serial.writeString("right\n");cout<<"static right\n";waitKey(waitTime);}
			else if(xMeanMean<frame.size().width/2 - frame.size().width*0.2){serial.writeString("left\n");cout<<"static left\n";waitKey(waitTime);}
			//waitKey(2000);
		}	
		//serial.writeString("right\n")
		xMeanFirst=0;
		xMeanSecond=0;
		cntsNumFirst=0;
		cntsNumSecond=0;

        imshow("Camera", frame);
		empty=Mat(288, 512, CV_8UC3, Scalar(255,255, 255));

        if(waitKey(1) == 27){
            //exit if ESC is pressed
            break;
        }
    }
    serial.closeDevice();
    return 0;
}
