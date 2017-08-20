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

void mouseCallBack(int event, int x, int y, int flags, void* data)
{
	auto pair = static_cast<std::pair<std::vector<std::vector<cv::Point>>*, std::vector<std::string>*>*>(data);
	auto contours = *(pair->first);
	auto figures = *(pair->second);

	for (int i =0;i<contours.size();++i)
	{
		if (pointPolygonTest(contours[i], Point(x, y), false) > 0)
		{
			cout << "There are " << figures[i] << endl;
		}
	}

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

	LaserTracing trace(frame.size());
	do
	{
		cap >> frame;
		imshow("Frame", frame);
		ch = waitKey(30);

		if (ch == 32)
		{
			cout << "Spcae handled" << endl;
			vector < vector<Point>> contours;
			vector<string> figures;
			detectFigure(trace.getTrace(), contours, figures);
			Mat inteface = trace.getTrace().clone();

			drawContours(inteface, contours, -1, Scalar::all(255), 2);
			auto pair = &make_pair(&contours, &figures);
			namedWindow("Detected figures");
			imshow("Detected figures", inteface);
			setMouseCallback("Detected figures", mouseCallBack, pair);

			waitKey();
			trace.clear();
		}

		Mat mask = colorSpaceLaserDetection(frame, h, s, v, l, true);
		trace.draw(frame, mask);
		imshow("trace", trace.getTrace());
		imshow("mask", mask);

		//getLaser(frame, background);
	}
	while (ch != 27);

	return 0;
}