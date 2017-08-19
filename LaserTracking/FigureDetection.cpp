#include "FigureDetection.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\ml.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace cv;
using namespace cv::ml;

static int morphSize = 50;
static int eps = 60;
static int acc = 7;
static int Rk = 1;
static int elispeEps = 10;
static std::string figuresName[5] = { "Triangle", "Rectangle", "Pentagon", "Hexagon", "Circle" };

std::string detectFigure(cv::Mat & mask)
{
	namedWindow("Panel");
	createTrackbar("MorphSize", "Panel", &morphSize, 150);
	createTrackbar("Eps", "Panel", &eps, 150);
	createTrackbar("Acc", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &Rk, 400);
	createTrackbar("elipseEps/10", "Panel", &elispeEps, 30);
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	string figure;
	do
	{
		Mat morph;
		dilate(maskGray, morph, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));

		vector < vector<Point> > contours;
		findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);
		Mat draw = mask.clone();

		for (auto i : contours)
		{
			cout << "Curve: " << isCurve(i) << endl;
			Rect area = boundingRect(i);
			eps = std::min(area.height, area.width) / 3;
			acc = eps / 10;
			vector<Point> approx;
			vector<Point> accuracyContour;
			approxPolyDP(i, approx, eps, true);
			approxPolyDP(i, accuracyContour, acc, true);
			drawContours(draw, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 1);
			drawContours(draw, vector<vector<Point>>{accuracyContour}, -1, Scalar(255, 255, 255), 1);
			//cout << "Accuracy contour corners - " << accuracyContour.size() << endl;
			if (isCircle(i, approx, accuracyContour, draw, figure))
			{		
					cout << "Found circle " << endl;
					figure = figuresName[4];
			}
			else
			{
				figure = figuresName[approx.size() - 3];
			}
			cout << "Founded " << approx.size() << " corners " << endl;
		}
		putText(draw, figure, Point(100, 100), FONT_HERSHEY_COMPLEX, 2, Scalar::all(255));
		imshow("figure", draw);
	} while (waitKey(30) != 27);
	destroyWindow("figure");
	destroyWindow("morph");
	destroyWindow("Panel");
	return figure;
}

bool isCurve(vector<Point> & points)
{
	cout << "area square = " << contourArea(points) << endl;
	return contourArea(points) > 0;
}

bool isCircle(std::vector<Point> i, std::vector<Point> approx, std::vector<Point> accuracyContour, cv::Mat& draw, std::string& figure)
{
	Point2f center;
	float radius;
	minEnclosingCircle(i, center, radius);
	//RotatedRect elipse = fitEllipse(i);

	cout << "R = " << radius << endl;
	int acc = accuracyContour.size();
	int apr = approx.size();
	float lenghtCircle = 2 * CV_PI*radius;
	float lenghtFigure = arcLength(i, true);
	float lenghtRatio = lenghtFigure / lenghtCircle;
	float squareFigure = contourArea(i);
	float squareCircle = CV_PI*radius*radius;
	float squareRatio = squareFigure / squareCircle;

	cout << "FigureS / Circle S = " << squareFigure << "/" << squareCircle << " = " << squareRatio << endl;
	circle(draw, center, radius, Scalar(255, 0, 0), 1);
	return (squareRatio > 0.7 || (apr <= 2 || apr > 6));
}