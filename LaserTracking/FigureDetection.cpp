#include "FigureDetection.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;

std::string detectFigure(cv::Mat & mask)
{
	int morphSize = 50;
	int eps = 50;
	namedWindow("Panel");
	createTrackbar("MorphSize", "Panel", &morphSize, 150);
	createTrackbar("Eps", "Panel", &eps, 150);
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	do
	{
		Mat morph;
		//morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		dilate(maskGray, morph, getStructuringElement(MORPH_RECT, Size(morphSize, morphSize)));
		imshow("morph", morph);

		vector < vector<Point> > contours;
		findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);

		Mat draw = mask.clone();
		for (auto i : contours)
		{
			vector<Point> approx;
			approxPolyDP(i, approx, eps, true);
			drawContours(draw, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 1);
			cout << "Founded " << approx.size() << " corners " << endl;
		}
		imshow("figure", draw);
	}
	while(waitKey(30) != 27);
	return "O_o";
}
