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

#include "LaserConsts.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

static int morphSize = 5;
static int eps = 60;
static int acc = 7;
static int Rk = 1;
static int elispeEps = 10;

void detectFigure(cv::Mat & mask, std::vector<std::vector<cv::Point>>& processedContours, std::vector<std::string>& figures)
{
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	string figure;
	Mat morph;
	morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(morphSize, morphSize)));

	vector < vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(morph, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	drawContours(morph, contours, -1, Scalar(255, 0, 0), 1);
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
				vector<Point> approxCont;
				string name = checkFigure(j, draw, approxCont);
				processedContours.push_back(approxCont);
				figures.push_back(name);
			}
		}
		else if (it == childrensIndex.end())
		{
			drawContours(draw, contours, i, Scalar(255, 0, 0), 1);
			int lenght = cv::arcLength(contours[i], true);
			stringstream str;
			string l;
			str << lenght;
			str >> l;
			string line = "Line (";
			line += l;
			line += "px)";
			processedContours.push_back(contours[i]);
			figures.push_back(line);
		}
	}
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

std::string checkFigure(std::vector<cv::Point> contour, cv::Mat& draw, std::vector<cv::Point>& approxContour)
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

	int corners = approx.size();
	name = figuresName[corners-3];
	
	if (corners == 4)
	{
		if (isRectangle(approx))
		{
			if (isSquare(approx, false))
			{
				name = figuresName[8];
			}
			else
			{
				name = figuresName[7];
			}
		}
	}

	int acc = accuaracy.size();
	int accThresh = 7;

	bool smooth = acc > accThresh;

	if (ratioS > circleRatioThresh && smooth)
	{
		Mat temp = Mat::zeros(draw.size(), CV_8UC1);
		circle(draw, center, radius, circleColor, 2);
		circle(temp, center, radius, Scalar::all(255), 2);
		vector<vector<Point>> tempCont;
		findContours(temp, tempCont, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		approxContour = tempCont.back();
		name = "Circle";
	}
	else if (ratioE > ellipseRatiothresh && smooth)
	{
		Mat temp = Mat::zeros(draw.size(), CV_8UC1);
		ellipse(draw, rect, ellipseColor, 2);
		ellipse(temp, rect, Scalar::all(255), 2);
		vector<vector<Point>> tempCont;
		findContours(temp, tempCont, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		approxContour = tempCont.back();
		name = "Ellipse";
	}
	else
	{
		drawContours(draw, std::vector<vector<Point>> { approx }, -1, figureColor, 2);
		approxContour = approx;
	}

	//putText(draw, name, contour[0], cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, textColor, 1);
	return name;
}

float ellipseSquare(cv::RotatedRect & el)
{
	return CV_PI*el.size.height*el.size.width / 4;
}

bool isRectangle(std::vector<cv::Point> figure)
{
	if (figure.size() != 4)
	{
		return false;
	}

	bool straightCorner = true;
	
	for (int i = 0; i < figure.size()-1 && straightCorner; i++)
	{
		Line a(figure[i], figure[i + 1]);
		Line b(figure[i+1], figure[(i + 2)%figure.size()]);
		float ang = angleBetween(a, b);
		bool isStraight = isEquil(ang, 90, 13);
		straightCorner = straightCorner && isStraight;
	}
	return straightCorner;
}

bool isSquare(std::vector<cv::Point> figure, bool checkRectangle)
{
	bool isFigureRectangle = !checkRectangle; // Rectangle, apriory
	if (!checkRectangle)
	{
		isFigureRectangle = isRectangle(figure) && figure.size() == 4;
	}
	
	bool isSquare = isFigureRectangle;
	for (int i = 0; i < figure.size()-1 && isSquare; i++)
	{
		float lenghtA = norm(figure[i] - figure[i+1]);
		float lenghtB = norm(figure[i+1] - figure[(i+2)%figure.size()]);
		 isSquare = isSquare && isEquil(lenghtA, lenghtB, (lenghtA + lenghtB)*0.1); //20% epsilon
	}
	return isSquare;
}

float angleBetween(Line a, Line b)
{
	float angleA = a.angle();
	float angleB = b.angle();
	float diffAngle = abs(angleA - angleB);
	return diffAngle;
}

bool isEquil(float a, float b, float epsilon)
{
	bool lower = a > b - epsilon;
	bool upper = a < b + epsilon;
	return lower && upper;
}
