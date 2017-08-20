#pragma once
#include <opencv2/core.hpp>
#include <string>


std::string detectFigure(cv::Mat& mask);

std::vector<std::vector<cv::Point>> getChildren(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int item, std::vector<int>& childrensIndex);

void checkFigure(std::vector<cv::Point> contour, cv::Mat& draw);

float ellipseSquare(cv::RotatedRect& el);