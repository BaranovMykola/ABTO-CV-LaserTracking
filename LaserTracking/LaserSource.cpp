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
#include "LaserTracing.h"
#include "FigureDetection.h"

using namespace cv;
using namespace std;



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

	LaserTracing trace(frame.size());
	do
	{
		cap >> frame;
		imshow("Frame", frame);
		ch = waitKey(30);
		Mat mask = colorSpaceLaserDetection(frame, h, s, v, l);
		trace.draw(frame, mask);
		imshow("trace", trace.getTrace());
		if (ch == 32)
		{
			cout << "Spcae handled" << endl;
			cout << "Figure detection... " << detectFigure(trace.getTrace()) << endl;
			trace.clear();
		}

		


		//getLaser(frame, background);
	}
	while (ch != 27);

	return 0;
}