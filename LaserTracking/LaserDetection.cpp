#include "LaserDetection.h"

#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <string>
#include <sstream>
#include <iostream>

#include "HSVLaserTrace.h"
#include "YUVLaserTrace.h"
#include "BGRLaserTrace.h"
#include "LaserDetection.h"
#include "MotionRecognition.h"

using namespace cv;
using namespace std;

void showHist(cv::Mat & frame)
{
	Mat planes[3];
	Mat hist[3];
	split(frame, planes);
	const int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	for (size_t i = 0; i < 3; i++)
	{
		calcHist(&planes[i], 1, 0, Mat(), hist[i], 1, &histSize, &histRange, range);
	}
	
	int hist_w = 1024; int hist_h = 1000;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	for (size_t i = 0; i < 3; i++)
	{
		normalize(hist[i], hist[i], 0, histImage.rows, NORM_MINMAX, -1, Mat());
	}
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist[0].at<float>(i - 1))),
			 Point(bin_w*(i), hist_h - cvRound(hist[0].at<float>(i))),
			 Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist[1].at<float>(i - 1))),
			 Point(bin_w*(i), hist_h - cvRound(hist[1].at<float>(i))),
			 Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist[2].at<float>(i - 1))),
			 Point(bin_w*(i), hist_h - cvRound(hist[2].at<float>(i))),
			 Scalar(0, 0, 255), 2, 8, 0);
	}
	imshow("Histogram", histImage);
}

cv::Mat findMinimumMotionArea(cv::Mat& mask, cv::Rect& area)
{
	Mat Points;
	if (mask.type() != CV_8UC1)
	{
		cv::cvtColor(mask, mask, CV_BGR2GRAY);
	}
	findNonZero(mask, Points);
	area = boundingRect(Points);
	Mat cropped = mask(area);
	return cropped;
}

std::vector<cv::Mat> splitToSimpleAreas(cv::Mat & area, cv::Mat& frame, cv::Rect thisArea)
{
	vector<Mat> regionsMat;
	if (!area.empty())
	{
		vector<vector<Point>> contours;
		findContours(area, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		Mat cMat = Mat::zeros(area.size(), CV_8UC1);
		/*drawContours(cMat, contours, -1, Scalar::all(255), 1);
		imshow("Contours", cMat);*/
		for (auto i : contours)
		{
			Rect region = boundingRect(i);
			region.x += thisArea.x;
			region.y += thisArea.y;
			Mat newArea = frame(region);
			regionsMat.push_back(newArea);
		}
	}
	return regionsMat;
}

cv::Mat colorSpaceLaserDetection(cv::Mat & frame, int h, int s, int v, int l, bool show)
{
	Mat hsv = hsvLaserDetect(frame, h,s,v);
	Mat yuv = yuvLaserDetect(frame, l);
	Mat motion = backgroundSubstract(frame);
	if (show)
	{
		imshow("hsv", hsv);
		imshow("yuv", yuv);
		imshow("motion", motion);
	}
	Mat mask;
	bitwise_and(hsv, yuv, mask);
	bitwise_and(mask, motion, mask);

	vector<vector<Point>> contours;
	findContours(mask, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	//Mat cMat = Mat::zeros(mask.size(), CV_8UC1);
	/*drawContours(cMat, contours, -1, Scalar::all(255), 1);
	imshow("Contours", cMat);*/
	Mat trace = Mat::zeros(mask.size(), CV_8UC1);
	if (contours.size() < 10)
	{
		
		for (auto i : contours)
		{
			Rect region = boundingRect(i);
			Mat newArea = frame(region);
			Mat newAreaMask = Mat::zeros(frame.size(), CV_8UC3);
			newArea.copyTo(newAreaMask(region));

			checkRedLaser(newAreaMask, frame);
			reduceMaximumSize(newArea, frame.size(), 0.01);
			if (countNonZero(newAreaMask) > 3 && countNonZero(newArea) > 0)
			{
				//newArea.copyTo(trace(region));
				trace += newAreaMask;
			}
		}
	}

	return trace;

}

cv::Mat reduceMaximumSize(cv::Mat & mask, cv::Size original, float percent)
{
	Mat cropped = findMinimumMotionArea(mask);
	Mat points;
	findNonZero(cropped, points);
	Rect area = boundingRect(points);
	if (cropped.rows*cropped.cols > original.height*original.width*percent ||
		area.height < 3 || area.width < 3 )
	{
		mask = 0;
	}
	return mask;
}
