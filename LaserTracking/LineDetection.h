#pragma once
#include <opencv2\core.hpp>

#include <vector>

#include "Line.h"

Line detectLine(std::vector<cv::Point> points, cv::Size frameSize);

float distanceLineToCurve(std::vector<cv::Point> curve, Line line, int maxX);