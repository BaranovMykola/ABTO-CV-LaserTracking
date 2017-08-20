#include "LaserTracing.h"

#include <opencv2\imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

LaserTracing::LaserTracing(cv::Size frameSize):
	from(0,0),
	to(0,0),
	state(Sleep)
{
	trace = Mat::zeros(frameSize, CV_8UC3);
}


LaserTracing::~LaserTracing()
{
}

void LaserTracing::draw(cv::Mat & frame, cv::Mat& mask)
{
	if (countNonZero(mask) > 0)
	{
		if (state == Sleep)
		{
			from = getLaserPoint(frame, mask);
			state = Draw;
			cout << "[LaserTracing] Stated drawing..." << endl;
			drawen = false;
			return;
		}
		else if (state == Draw)
		{
			drawen = true;
			to = getLaserPoint(frame, mask);
			line(trace, from, to, Scalar(0,0,255), 3);
			from = to;
		}
		else
		{
			cout << "[LaserTracing] Unknown tracing state. Error while drawing" << endl;
		}
	}
	else if(state != Sleep)
	{
			state = Sleep;
			cout << "[LaserTracing] Ended drawing..." << endl;
			if (!drawen) cout << "\t Nothing drawen" << endl;
			drawen = false;
			return;
	}
}

Point LaserTracing::getLaserPoint(cv::Mat & frame, cv::Mat & mask)
{
	Point center;
	Mat maskedFrame;
	frame.copyTo(maskedFrame, mask);
	Mat planes[3];
	split(maskedFrame, planes);
	minMaxLoc(planes[2], 0, 0, 0, &center);
	return center;
}

cv::Mat LaserTracing::getTrace() const
{
	return trace;
}

void LaserTracing::clear()
{
	trace = Mat::zeros(trace.size(), CV_8UC3);
}
