#include "LineDetection.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include <iostream>
#include <algorithm>
#include <climits>
#include <iterator>

using namespace cv;
using namespace std;

Line detectLine(std::vector<cv::Point> points, cv::Size frameSize)
{
	int hughThresh = 1;

	Rect area = boundingRect(points);
	Mat areaImg = Mat::zeros(frameSize, CV_8UC1);
	drawContours(areaImg, vector<vector<Point>>{points}, -1, Scalar::all(255), 5);
	vector<Vec2f> lines;
	/*while (lines.size() != 1)
	{
		HoughLines(areaImg, lines, 2, CV_PI / 180, hughThresh);
		++hughThresh;
	}*/

	int maxThresh = sqrt(pow(frameSize.height, 2) + pow(frameSize.width, 2));
	vector<int> thresh;
	int first = 1;
	std::generate_n(std::back_inserter(thresh), maxThresh, [&]()
	{
		return first++;
	});
	int index;
	/*auto res= binary_search(thresh.begin(), thresh.end(), 1, [&](int s, int t)
	{
		HoughLines(areaImg, lines, 2, CV_PI / 180, s);
		index = s;
		return lines.size() > 1;
	});*/

	auto res = upper_bound(thresh.begin(), thresh.end(), 1, [&](int s, int t)
	{
		HoughLines(areaImg, lines, 2, CV_PI / 180, t);
		index = t;
		return lines.size() < 1;
	});
	lines.clear();
	HoughLines(areaImg, lines, 2, CV_PI / 180, *res-1);
	Mat draw = areaImg.clone();
	Line item;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(draw, pt1, pt2, Scalar::all(100), 3);
		item = Line(pt1, pt2);
	}
	imshow("draw", draw);
	int maxDeviation = distanceLineToCurve(points, item, frameSize.width);
	cout << "Threshould is " << hughThresh << endl;
	cout << "Max deviation = " << maxDeviation << endl;
	if (maxDeviation > std::max(frameSize.width, frameSize.height)*0.05) //5%
	{
		cout << "There are no lines" << endl;
		return Line();
	}
	//waitKey();
	destroyWindow("draw");
	return item;
}

float distanceLineToCurve(std::vector<cv::Point> curve, Line line, int maxX)
{
	float maxDist = 0;
	for (auto i : curve)
	{
		Point close;
		Point diff;
		float minDist = FLT_MAX;
		if (!line.vertical)
		{
			for (int x = 0; x < maxX; x++)
			{
				Point linePoint = Point(x, line.a*x + line.b);
				diff = i - linePoint;
				int deviation = norm(diff);
				if (minDist > deviation)
				{
					close = linePoint;
				}
				minDist = deviation < minDist ? deviation : minDist;
			}
		}
		else
		{
			int deviation = abs(i.x - line.b);
			minDist = deviation < minDist ? deviation : minDist;
		}
		maxDist = maxDist < minDist ? minDist : maxDist;
	}
	return maxDist;
}