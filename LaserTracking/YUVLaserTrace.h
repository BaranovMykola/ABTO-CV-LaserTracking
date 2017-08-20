#pragma once
#include <opencv2\core.hpp>

/*@brief

Return laser points based on YUV color space
*/
cv::Mat yuvLaserDetect(cv::Mat& frame, int lightThresh = 170);

