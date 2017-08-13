#include "LaserTrace.h"

#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <string>
#include <sstream>
#include <iostream>

#include "HSVLaserTrace.h"
#include "YUVLaserTrace.h"
#include "BGRLaserTrace.h"

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

cv::Mat getLaser(cv::Mat & frame, cv::Mat & background)
{
	Mat diff;
	Mat mask = Mat::zeros(frame.size(), CV_8UC1);
	Mat blur;
	GaussianBlur(frame, blur, Size(5, 5), 3);
	absdiff(blur, background, diff);
	threshold(diff, mask, 50, 255, THRESH_BINARY);
	
	Rect area;
	auto cropped = findMinimumMotionArea(mask, area);

	auto objects = splitToSimpleAreas(cropped, frame, area);

	for (auto i : objects)
	{
		computeRed(i);
	}

	return diff;
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
		drawContours(cMat, contours, -1, Scalar::all(255), 1);
		imshow("Contours", cMat);
		for (auto i : contours)
		{
			Rect region = boundingRect(i);
			region.x += thisArea.x;
			region.y += thisArea.y;
			Mat newArea = frame(region);
			regionsMat.push_back(newArea);
			/*showHist(newArea);
			imshow("area", newArea);*/
			/*waitKey();*/
		}
	}
	return regionsMat;
}

float computeRed(cv::Mat & area)
{
	Mat plane[3];
	split(area, plane);
	Mat res(area.size(), CV_32FC1);
	res = 2 * plane[2];
	res = plane[1] - plane[0];
	normalize(res, res, 0, 255, NORM_MINMAX);
	return 0;
}

cv::Mat colorSpaceLaserDetection(cv::Mat & frame)
{
	Mat hsv = hsvLaserDetect(frame);
	Mat yuv = yuvLaserDetect(frame);
	Mat mask;
	bitwise_and(hsv, yuv, mask);
	checkRedLaser(mask, frame);
	return mask;
}
