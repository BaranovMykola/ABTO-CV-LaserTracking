#pragma once
#include <opencv2\core.hpp>

/*@brief

Test if current mask represent laser. Based on red color
@param mask Laser mask
@param frame Original frame. Used for color comparing
*/
cv::Mat checkRedLaser(cv::Mat& mask, cv::Mat& frame);