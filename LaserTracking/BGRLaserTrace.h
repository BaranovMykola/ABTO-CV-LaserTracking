#pragma once
#include <opencv2\core.hpp>

cv::Mat checkRedLaser(cv::Mat& mask, cv::Mat& frame);

float computeRed(cv::Mat& area);