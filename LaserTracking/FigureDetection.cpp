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

static int morphSize = 40;
static int eps = 60;
static int acc = 7;
static int Rk = 1;
static int elispeEps = 10;
static std::string figuresName[7] = { "Triangle", "Rectangle", "Pentagon", "Hexagon", "Heptagon", "Octagon", "Nonagon" };
static Scalar figureColor(0, 255, 0);
static Scalar ellipseColor(255, 0, 0);
static Scalar circleColor(255, 255, 0);
static Scalar textColor(255, 255, 255);

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
		morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));

		vector < vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(morph, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);
		Mat draw = mask.clone();
		vector<int> childrensIndex;

		for (int i = 0; i < contours.size(); i++)
		{
			vector <int> currentChildrenIndex;
			auto children = getChildren(contours, hierarchy, i, currentChildrenIndex);
			childrensIndex.insert(childrensIndex.end(), currentChildrenIndex.begin(), currentChildrenIndex.end());
			auto it = std::find(childrensIndex.begin(), childrensIndex.end(), i);
			if (!children.empty())
			{
				for (auto j : children)
				{
					checkFigure(j, draw);
				}
			}
			else if (it == childrensIndex.end())
			{
				drawContours(draw, contours, i, Scalar(255, 0, 0), 1);
				float lenght = cv::arcLength(contours[i], true);
				stringstream str;
				string l;
				str << lenght;
				str >> l;

				Point center = contours[i][0];
				putText(draw, l, center, cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255));
			}
		}
		imshow("figure", draw);
	}
	while (waitKey(30) != 27);
	destroyWindow("figure");
	destroyWindow("morph");
	destroyWindow("Panel");
	return figure;
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
	const float circleRatioThresh = 0.65;
	const float ellipseRatiothresh = 0.95;

	Rect area = boundingRect(contour);
	int aprEps = std::min(area.width, area.height)*0.2;
	int accEps = aprEps*0.3;

	string name;
	vector<Point> approx;
	vector<Point> accuaracy;

	approxPolyDP(contour, approx, aprEps, true);
	approxPolyDP(contour, accuaracy, accEps, true);

	Point2f center;
	float radius;
	minEnclosingCircle(contour, center, radius);
	auto rect = fitEllipse(contour);

	float circleS = CV_PI*radius*radius;
	float figureS = contourArea(contour);
	float ratioS = figureS / circleS;
	float ratioE = figureS / ellipseSquare(rect);

	cout << "figureS/circleS = " << figureS << "/" << circleS << " = " << ratioS << endl;
	cout << "figureS/ellipseS = " << figureS << "/" << ellipseSquare(rect) << " = " << ratioE << endl;
	cout << endl;

	name = figuresName[approx.size() - 3];

	int acc = accuaracy.size();
	int accThresh = 7;

	bool smooth = acc > accThresh;

	if (ratioS > circleRatioThresh && smooth)
	{
		circle(draw, center, radius, circleColor, 2);
		name = "Circle";
	}
	else if (ratioE > ellipseRatiothresh && smooth)
	{
		ellipse(draw, rect, ellipseColor, 2);
		name = "Elipse";
	}
	else
	{
		drawContours(draw, std::vector<vector<Point>> { approx }, -1, figureColor, 2);
	}

	putText(draw, name, contour[0], cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, textColor, 1);
}

float ellipseSquare(cv::RotatedRect & el)
{
	return CV_PI*el.size.height*el.size.width / 4;
}
