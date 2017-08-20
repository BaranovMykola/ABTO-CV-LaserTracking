#pragma once
#include <opencv2\core.hpp>

#include "LaserConsts.h"

/*@brief

Calculate and show histogram of 3-channels image
*/
void showHist(cv::Mat & frame);

/*@brief 

Approximates rectangle to mask and returns cropped mask
*/
cv::Mat findMinimumMotionArea(cv::Mat& mask, cv::Rect& area = default);

/*@brief 

Split mask to several nonintersection regions

@param thisArea Represents position $area$ on source image
*/
std::vector<cv::Mat> splitToSimpleAreas(cv::Mat & area, cv::Mat& frame, cv::Rect thisArea);

/*@brief

Return laser mask from source image

@param frame Source image
@param h,s,v,l Hue, Saturation, Value, Lightness thresholds for HSV and YUV
color spaces
*/
cv::Mat colorSpaceLaserDetection(cv::Mat& frame, int h, int s, int v, int l, bool = false);

/*@brief

If mask are bigger than certain size it will be replaced by zero mask
*/
cv::Mat reduceMaximumSize(cv::Mat& mask, cv::Size original, float percent);