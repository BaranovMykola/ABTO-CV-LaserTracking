#pragma once
#include <opencv2\core.hpp>

#include <vector>

#include "Line.h"

/*@brief

Detects line from curves using Hough transfrom

@param points Curve
@param frameSize Size of image where to find line
*/
Line detectLine(std::vector<cv::Point> points, cv::Size frameSize);

/*@brief

Measures max deviation curve from line

@pram maxX max for iteration by OX
*/
float distanceLineToCurve(std::vector<cv::Point> curve, Line line, int maxX);

/*@brief

Calculates line corners based on curves
*/
void getLineEdges(std::vector<cv::Point> curve, Line line, cv::Point& pt1, cv::Point& pt2, int maxX);

/*@brief

Returns point with minimal way to line
*/
cv::Point getMinimum(Line from, cv::Point to, int maxX);