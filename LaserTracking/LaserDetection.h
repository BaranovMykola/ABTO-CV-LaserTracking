#pragma once
#include <opencv2\core.hpp>

void showHist(cv::Mat & frame);

cv::Mat getLaser(cv::Mat& frame, cv::Mat& background);

static cv::Rect default;

cv::Mat findMinimumMotionArea(cv::Mat& mask, cv::Rect& area = default);

std::vector<cv::Mat> splitToSimpleAreas(cv::Mat & area, cv::Mat& frame, cv::Rect thisArea);

cv::Mat colorSpaceLaserDetection(cv::Mat& frame, int,int,int,int);

cv::Mat reduceMaximumSize(cv::Mat& mask, cv::Size original, float percent);