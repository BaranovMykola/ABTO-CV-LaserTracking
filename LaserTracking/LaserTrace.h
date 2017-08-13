#pragma once
#include <opencv2\core.hpp>

void showHist(cv::Mat & frame);

cv::Mat getLaser(cv::Mat& frame, cv::Mat& background);

cv::Mat findMinimumMotionArea(cv::Mat& mask, cv::Rect& area);

std::vector<cv::Mat> splitToSimpleAreas(cv::Mat & area, cv::Mat& frame, cv::Rect thisArea);

float computeRed(cv::Mat& area);