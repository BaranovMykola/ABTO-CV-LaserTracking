#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"

#include <iostream>
#include <algorithm>

#include "BGRLaserTrace.h"
#include "HSVLaserTrace.h"
#include "YUVLaserTrace.h"
#include "LaserDetection.h"
#include "MotionRecognition.h"
#include "ProjectorCalibration.h"
#include "LaserTracing.h"
#include "LineDetector.h"

using namespace cv;
using namespace std;

void detectFigure(Mat& trace)
{
	int accuracy = 34;
	destroyAllWindows();
	namedWindow("Panel");
	createTrackbar("accuracy", "Panel", &accuracy, 100);
	do
	{
		imshow("Trace", trace);
		vector< vector<Point> > contours;
		findContours(trace, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		Mat contoursImg(trace.size(), CV_8UC3);
		contoursImg = Scalar::all(0);
		drawContours(contoursImg, contours, -1, Scalar(255, 0, 0));
		imshow("contours", contoursImg);

		vector<Point> approx;
		for (size_t i = 0; i < contours.size(); i++)
		{
			std::cout << i << ")";
			approxPolyDP(Mat(contours[i]), approx, cv::arcLength(Mat(contours[i]), true)*accuracy / 1000.0, true);
			drawContours(contoursImg, vector<vector<Point>>({ approx }), 0, Scalar(0, 0, 255), 1);
			for (auto j : approx)
			{
				circle(contoursImg, j, 1, Scalar::all(255), -1);
			}
			std::cout << "There are " << approx.size() << "-angle" << std::endl;
			imshow("contours", contoursImg);
		}
		cout << std::endl;
	} while (waitKey(30) != 27);
	trace = Mat::zeros(trace.size(), CV_8UC1);
	destroyAllWindows();
}

int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "Camera didn't open" << endl;
		system("pause");
		return -1;
	}
	//namedWindow("Histogram", WINDOW_AUTOSIZE);

	int ch;
	
	Mat frame;
	cap >> frame;

	Mat background;
	Mat diff;
	Mat mask;

	cap >> background;

	Mat blur;
	GaussianBlur(background, blur, Size(5, 5), 3);
	background = blur;

	int h = 182;
	int s = 121;
	int v = 229;
	int l = 203;

	namedWindow("Laser");
	createTrackbar("H", "Laser", &h, 255);
	createTrackbar("S", "Laser", &s, 255);
	createTrackbar("V", "Laser", &v, 255);
	createTrackbar("L", "Laser", &l, 255);


	//do
	//{
	//	cap >> frame;
	//	imshow("Frame", frame);
	//	ch = waitKey(30);
	//	Mat mask = backgroundSubstract(frame);
	//	if (ch == 32)
	//	{
	//		cout << "Spcae handled" << endl;
	//		//space();
	//	}

	//	
	//	Mat mask;
	//	bitwise_and(colorSpaceLaserDetection(frame,h,s,v,l), mask, mask);
	//	imshow("mask", mask);

	LaserTracing trace(frame.size());

	do
	{
		cap >> frame;
		imshow("Frame", frame);
		ch = waitKey(30);
		//Mat mask = backgroundSubstract(frame);
		Mat mask = colorSpaceLaserDetection(frame, h, s, v, l);
		if (ch == 32)
		{
			cout << "Spcae handled" << endl;
			detectLine(frame, mask);
			waitKey();
			trace.clear();
		}

		
		trace.draw(frame, mask);
		imshow("trace", trace.getTrace());
		imshow("res", mask);


	//	//getLaser(frame, background);
	}
	while (ch != 27);

	/*ProjectorCalibration proj(Size(300, 300));
	proj.drawControlPoints(1);*/


	return 0;
}
