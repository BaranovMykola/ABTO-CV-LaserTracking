#pragma once
#include <opencv2\core.hpp>
#include <vector>

/*@brief

Use HSV color space to detect laser without substracting background
*/
cv::Mat hsvLaserDetect(cv::Mat& frame, int hThresh = 250, int shresh = 40, int vhresh = 40);
