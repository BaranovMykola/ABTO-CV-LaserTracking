#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"

#include <iostream>
#include <algorithm>
#include <tuple>

#include "BGRLaserTrace.h"
#include "HSVLaserTrace.h"
#include "YUVLaserTrace.h"
#include "LaserDetection.h"
#include "MotionRecognition.h"
#include "LaserTracing.h"
#include "FigureDetection.h"
#include "LaserConsts.h"

using namespace cv;
using namespace std;

void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat& mat, int item, int thinknes)
{
	Scalar color = shapeColors.find(figures[item])->second;
	drawContours(mat, contours, item, color, 5);
}

void mouseCallBack(int event, int x, int y, int flags, void* data)
{
	auto cont = static_cast<std::tuple<std::vector<std::vector<cv::Point>>*, std::vector<std::string>*, cv::Mat*>*>(data);
	auto contours = *std::get<0>(*cont);
	auto figures = *std::get<1>(*cont);
	auto mat = std::get<2>(*cont)->clone();

	for (int i =0;i<contours.size();++i)
	{
		if (pointPolygonTest(contours[i], Point(x, y), false) > 0)
		{
			putText(mat, figures[i], contours[i][0], cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255), 1);
			drawColorShape(contours, figures, mat, i, 5);
		}
	}
	imshow("Detected figures", mat);

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

			
			for (int i = 0; i < contours.size(); i++)
			{
				drawColorShape(contours, figures, inteface, i, 5);
			}
			auto pair = &make_tuple(&contours, &figures, &inteface);

			namedWindow("Detected figures");
			imshow("Detected figures", inteface);
			setMouseCallback("Detected figures", mouseCallBack, pair);
			waitKey();
			destroyWindow("Detected figures");
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