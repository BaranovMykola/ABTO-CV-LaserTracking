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
#include "Cmd.h"

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

	int timeWait = 3;
	bool systemInfo = false;

	/*namedWindow("Laser");
	createTrackbar("H", "Laser", &h, 255);
	createTrackbar("S", "Laser", &s, 255);
	createTrackbar("V", "Laser", &v, 255);
	createTrackbar("L", "Laser", &l, 255);*/

	LaserTracing trace(frame.size());
	do
	{
		cap >> frame;
		if (frame.empty())
		{
			cout << "Error while reading frame from camera..." << endl;
			cout << "Exiting..." << endl;
			break;
		}
		imshow("Camera", frame);
		ch = waitKey(30);
		if (ch == 32 || trace.timeElapsed(timeWait) && trace.isAnythingDrawn())
		{
			//cout << "Spcae handled" << endl;
			vector < vector<Point>> contours;
			vector<string> figures;
			detectFigure(trace.getTrace(), contours, figures);
			Mat inteface = trace.getTrace().clone();
			vector<vector<Point>> animation;
			for (int i = 0; i < contours.size(); i++)
			{
				drawColorShape(contours, figures, inteface, i, 1);
				if (shapeColors.find(figures[i]) == shapeColors.end())
				{
					auto item = detectLine(contours[i], frame.size());
					if (item != Line())
					{
						figures[i] = callLine(item);
						contours[i].clear();
						contours[i].push_back(item.pt0);
						contours[i].push_back(item.pt1);
						Point pt1Shift = getVolumePoint(item.pt1, item);
						contours[i].push_back(pt1Shift);
						Point pt2Shift = getVolumePoint(item.pt0, item);
						contours[i].push_back(pt2Shift);
						contours[i].push_back(item.pt0);
						
					}
				}
				animation.push_back(contours[i]);
			}
			auto pair = &make_tuple(&animation, &figures, &inteface);
			destroyWindow("Camera");
			destroyWindow("Trace");
			namedWindow("Detected figures");
			imshow("Detected figures", inteface);
			setMouseCallback("Detected figures", mouseCallBack, pair);
			//waitKey();

			LaserTracing waiter(frame.size());
			while (!waiter.isAnythingDrawn())
			{
				waitKey(1);
				if (frame.empty())
				{
					break;
				}
				cap >> frame;
				Mat waiterMask = colorSpaceLaserDetection(frame, h, s, v, l);
				waiter.draw(frame, waiterMask);
			}

			destroyWindow("Detected figures");
			trace.clear();
		}
		if (ch == 96)
		{
			string command;
			destroyAllWindows();
			cout << "Console mode..." << endl;
			do
			{
				cout << ">> ";
				command = Cmd::ask();
				if (command == "-time")
				{
					cin >> timeWait;
					cout << "Autodetect figures delay setted to " << timeWait << " seconds" << endl;
				}
				else if (command == "-info")
				{
					cin >> systemInfo;
					cout << "Showing system information is setted to " << systemInfo << endl;
				}
				else if(command == "-help" || command == "-h")
				{
					cout << "-time [int]\tAutodetect figures delay" << endl
						<< "-info [bool]\tShow system laser info" << endl
						<< "-help\t Show this window" << endl;
				}
				else
				{
					cout << "Unknow command. Type -help for more info" << endl;
				}
			}
			while (command != "-q" && command != "-exit");
			
		}

		Mat mask = colorSpaceLaserDetection(frame, h, s, v, l, systemInfo);
		trace.draw(frame, mask);
		imshow("Trace", trace.getTrace());
		//imshow("mask", mask);
	}
	while (ch != 27);

	return 0;
}