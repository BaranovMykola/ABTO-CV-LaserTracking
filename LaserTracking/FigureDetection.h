#pragma once
#include <opencv2/core.hpp>
#include <string>


std::string detectFigure(cv::Mat& mask);

bool isCurve(std::vector<cv::Point>& points);

bool isCircle(std::vector<cv::Point> i, std::vector<cv::Point> approx, std::vector<cv::Point> accuracyContour, cv::Mat & draw, std::string & figure);
