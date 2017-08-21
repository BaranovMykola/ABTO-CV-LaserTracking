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
#include "FigureGUI.h"
#include "LineDetection.h"

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

	int ch;
	Mat frame;
	Mat mask;

	cap >> frame;

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
				drawColorShape(contours, figures, inteface, i, 1);
				if (shapeColors.find(figures[i]) == shapeColors.end())
				{
					auto item = detectLine(contours[i], frame.size());
					if (item != Line())
					{
						figures[i] = callLine(item);
					}
				}
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
	}
	while (ch != 27);

	return 0;
}