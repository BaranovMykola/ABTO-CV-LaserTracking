#pragma once
#include <opencv2\core.hpp>

#include "LaserConsts.h"

/*@brief

Substract frame from bacground and detect motions
*/
cv::Mat backgroundSubstract(cv::Mat& frame);