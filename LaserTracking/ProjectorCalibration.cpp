#include "ProjectorCalibration.h"

#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

#include "LaserDetection.h"

using namespace std;
using namespace cv;

ProjectorCalibration::ProjectorCalibration()
{
}


ProjectorCalibration::~ProjectorCalibration()
{
}

void ProjectorCalibration::drawControlPoints(int camera)
{
	VideoCapture cap(camera);
	Mat frame;
	cap >> frame;
	Mat mask = Mat::zeros(frame.size(), CV_8UC1);
	Mat zero = Mat::zeros(frame.size(), CV_8UC3);
	zero = 255;
	int ch;

	int h = 182;
	int s = 121;
	int v = 229;
	int l = 203;

	do
	{
		ch = waitKey(30);
		cap >> frame;
		Mat currentMask = colorSpaceLaserDetection(frame, h, s, v, l);
		mask += currentMask;
		imshow("frame", frame);
		imshow("mask", mask);
		if (countNonZero(currentMask) == 0)
		{
			waitKey();
		}
	}
	while(ch != 27);
}
