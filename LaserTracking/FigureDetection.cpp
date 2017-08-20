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
#include <sstream>
#include <string>

using namespace std;
using namespace cv;
using namespace cv::ml;

static int morphSize = 20;
static int eps = 60;
static int acc = 7;
static int Rk = 1;
static int elispeEps = 10;
static std::string figuresName[7] = { "Triangle", "Rectangle", "Pentagon", "Hexagon", "Heptagon", "Octagon", "Nonagon" };

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
		//dilate(maskGray, morph, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));

		vector < vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(morph, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);
		Mat draw = mask.clone();

		/*for (auto i : contours)
		{
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
		putText(draw, figure, Point(100, 100), FONT_HERSHEY_COMPLEX, 2, Scalar::all(255));*/

		vector<int> childrensIndex;

		for (int i = 0; i < contours.size(); i++)
		{
			vector <int> currentChildrenIndex;
			auto children = getChildren(contours, hierarchy, i, currentChildrenIndex);
			childrensIndex.insert(childrensIndex.end(), currentChildrenIndex.begin(), currentChildrenIndex.end());
			auto it = std::find(childrensIndex.begin(), childrensIndex.end(), i);
			if (!children.empty())
			{
			//	drawContours(draw, children, -1, Scalar(255,255,255), 1);
				for (auto j : children)
				{
					checkFigure(j, draw);
				}
			}
			else if(it == childrensIndex.end())
			{
				drawContours(draw, contours, i, Scalar(255, 0, 0), 1);
				float lenght = cv::arcLength(contours[i], true);
				stringstream str;
				string l;
				str << lenght;
				str >> l;

				Point center = contours[i][0];
							center.y += 15;
				putText(draw, l, center, cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255));
			}
		}

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

std::vector<std::vector<cv::Point>> getChildren(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int item, vector<int>& childrensIndex)
{
	vector<vector<Point> > children;
	int next = hierarchy[item][2];
	while (next != -1)
	{
		children.push_back(contours[next]);
		childrensIndex.push_back(next);
		next = hierarchy[next][0];
	}
	return children;
}

void checkFigure(std::vector<cv::Point> contour, cv::Mat & draw)
{
	Rect area = boundingRect(contour);
	int aprEps = std::min(area.width, area.height)*0.2;
	int accEps = aprEps*0.3;
	string name;

	vector<Point> approx;
	vector<Point> accuaracy;

	approxPolyDP(contour, approx, aprEps, true);
	approxPolyDP(contour, accuaracy, accEps, true);

	/*drawContours(draw, std::vector<vector<Point>> { approx }, -1, Scalar(0, 255, 0), 2);
	drawContours(draw, std::vector<vector<Point>> { accuaracy }, -1, Scalar(255, 255, 255), 2);*/

	Point2f center;
	float radius;
	minEnclosingCircle(contour, center, radius);
	auto rect = fitEllipse(contour);
	
	float circleS = CV_PI*radius*radius;
	float figureS = contourArea(contour);
	float ratioS = figureS/circleS;
	float ratioE = figureS / ellipseSquare(rect);

	cout << "figureS/circleS = " << figureS << "/" << circleS << " = " << ratioS << endl;
	cout << "figureS/ellipseS = " << figureS << "/" << ellipseSquare(rect) << " = " << ratioE << endl;
	cout << endl;

	name = figuresName[approx.size()-3];

	int acc = accuaracy.size();
	int accThresh = 7;

	bool smooth = acc > accThresh;

/*	if (ratioS > 0.65 && ratioE > 0.95 && smooth)
	{
		if (ratioS > 0.65)
		{
			circle(draw, center, radius, Scalar(255, 40, 100), 2);
			name = "Circle";
		}
		else
		{
			ellipse(draw, rect, Scalar(255, 40, 100), 2);
			name = "Elipse";
		}
	}

	else */if(ratioS > 0.65 && smooth)
	{
		circle(draw, center, radius, Scalar(255, 40, 100), 2);
		name = "Circle";
	}
	else if (ratioE > 0.95 && smooth)
	{
		ellipse(draw, rect, Scalar(255, 40, 100), 2);
		name = "Elipse";
	}
	else
	{

		drawContours(draw, std::vector<vector<Point>> { approx }, -1, Scalar(0, 255, 0), 2);
	}


	Point t = contour[0];
	putText(draw, name, t, cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255), 1);
}

float ellipseSquare(cv::RotatedRect & el)
{
	return CV_PI*el.size.height*el.size.width/4;
}
