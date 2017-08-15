#include "ProjectorCalibration.h"

#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\imgproc.hpp>

#include <numeric>
#include <iostream>

#include "LaserDetection.h"

using namespace std;
using namespace cv;

ProjectorCalibration::ProjectorCalibration(cv::Size resolution) :
	projectorResolution(resolution)
{
}


ProjectorCalibration::~ProjectorCalibration()
{
}

void ProjectorCalibration::drawControlPoints(int camera)
{
	pointsCountCalibration = AffinePoints;
	VideoCapture cap(camera);
	Mat frame;
	cap >> frame;
	for (size_t i = 0; i < 15; i++)
	{
		cap >> frame;
		colorSpaceLaserDetection(frame, 0,0,0,0);
	}
	Mat mask = Mat::zeros(frame.size(), CV_8UC1);
	Mat zero = Mat::zeros(frame.size(), CV_8UC3);
	zero = 255;
	int ch;

	int h = 182;
	int s = 121;
	int v = 229;
	int l = 203;

	
	LaserTracing trace(frame.size());
	Capture status = Capture::Wait;
	do
	{
		ch = waitKey(30);
		cap >> frame;
		Mat currentMask = colorSpaceLaserDetection(frame, h, s, v, l);
		trace.draw(frame, currentMask);
		imshow("frame", frame);
		int nonZero = countNonZero(currentMask);
		if (nonZero == 0 && status == Capture::Calibration)
		{
			status = Capture::Wait;
			pointApproximation(frame, trace);
		}
		else if (nonZero > 0 && status == Wait) 
		{
			status = Capture::Calibration;
		}
	}
	while(pointsCountCalibration > 0);
	projectorDrawPoint(frame.size());
}

void ProjectorCalibration::pointApproximation(Mat & frame, LaserTracing& trace)
{
	vector<Point> points;
	findNonZero(trace.getTraceMask(), points);
	Point2f sum = ((std::accumulate(points.begin(), points.end(), Point2f(0, 0), [](Point2f sum, Point i) { return sum + (Point2f)i; })));
	Point mean = sum / (double)points.size();
	circle(frame, mean, 5, Scalar(0, 0, 255), -1);
	imshow("frame", frame);
	cout << "Point detected at " << mean << endl;
	trace.clear();
	if (waitKey() != 27)
	{
		LaserPoints.push_back(mean);
		cout << "Point saved..." << endl;
		--pointsCountCalibration;
	}
	else
	{
		cout << "Point rejected";
	}
}

void ProjectorCalibration::projectorDrawPoint(cv::Size frameSize)
{
	namedWindow("Projector", WINDOW_FULLSCREEN);
	projectorImage = Mat(frameSize, CV_8UC3);
	projectorImage = Scalar::all(0);
	imshow("Projector", projectorImage);
	
	VideoCapture cap(1);
	Mat bacground;
	Mat frame;
	cap >> frame;

	cap >> bacground;
	imwrite("background.jpg", bacground);

	circle(frame, LaserPoints.back(), 30, Scalar(255, 0, 0), -1);
	waitKey();
	cap >> frame;
	cap >> frame;
	imwrite("frame.jpg", frame);

	projectorSearchPoint(bacground, frame);

	waitKey();
}

void ProjectorCalibration::projectorSearchPoint(cv::Mat & background, cv::Mat & frame)
{

	Mat bBlur;
	Mat fBlur;
	GaussianBlur(background, bBlur, Size(9, 9), 6);
	GaussianBlur(frame, fBlur, Size(9, 9), 6);
	imshow("background", bBlur);
	imshow("frame", fBlur);

	Mat diff;
	absdiff(bBlur, fBlur, diff);
	imshow("diff", diff);
	waitKey();
}
