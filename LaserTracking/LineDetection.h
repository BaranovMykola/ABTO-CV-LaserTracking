#pragma once
#include <opencv2\core.hpp>

#include <vector>

#include "Line.h"

Line detectLine(std::vector<cv::Point> points, cv::Size frameSize);

float distanceLineToCurve(std::vector<cv::Point> curve, Line line, int maxX);

void getLineEdges(std::vector<cv::Point> curve, Line line, cv::Point& pt1, cv::Point& pt2, int maxX);

cv::Point getMinimum(Line from, cv::Point to, int maxX);