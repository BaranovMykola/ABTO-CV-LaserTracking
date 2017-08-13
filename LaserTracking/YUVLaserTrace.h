#pragma once
#include <opencv2\core.hpp>

cv::Mat yuvLaserDetect(cv::Mat& frame, int lightThresh = 170);

