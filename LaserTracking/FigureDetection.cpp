#include "FigureDetection.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <iostream>

using namespace std;
using namespace cv;

	static int morphSize = 50;
	static int eps = 60;
	static int acc = 7;
	static int Rk = 1;
	static std::string figuresName[5] = { "Triangle", "Rectangle", "Pentagon", "Hexagon", "Circle" };
std::string detectFigure(cv::Mat & mask)
{
	namedWindow("Panel");
	createTrackbar("MorphSize", "Panel", &morphSize, 150);
	createTrackbar("Eps", "Panel", &eps, 150);
	createTrackbar("Acc", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &Rk, 400);
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	do
	{
		Mat morph;
		dilate(maskGray, morph, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		imshow("morph", morph);

		vector < vector<Point> > contours;
		findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);
		string figure;
		Mat draw = mask.clone();
		for (auto i : contours)
		{
			vector<Point> approx;
			vector<Point> accuracyContour;
			approxPolyDP(i, approx, eps, true);
			approxPolyDP(i, accuracyContour, acc, true);
			drawContours(draw, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 1);
			//drawContours(draw, vector<vector<Point>>{accuracyContour}, -1, Scalar(255, 255, 255), 1);
			cout << "Accuracy contour corners - " << accuracyContour.size() << endl;
				Point2f center;
				float radius;
				minEnclosingCircle(i, center, radius);
				cout << "R = " << radius;
			if (accuracyContour.size() > 12*(radius > 100 ? 1 : radius/100.0))
			{
				circle(draw, center, radius, Scalar(255, 0, 0), 1);
				//cout << "Founded circle " << endl;
				figure = figuresName[4];
			}
			else
			{
				figure = figuresName[approx.size()-3];
			}
			//cout << "Founded " << approx.size() << " corners " << endl;
		}
		putText(draw, figure, Point(100, 100), FONT_HERSHEY_COMPLEX, 2, Scalar::all(255));
		imshow("figure", draw);
	}
	while(waitKey(30) != 27);
	destroyWindow("figure");
	destroyWindow("morph");
	destroyWindow("Panel");
	return "O_o";
}