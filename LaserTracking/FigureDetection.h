#pragma once
#include <opencv2/core.hpp>
#include <string>

/*@brief

Detects figures drawn at $mask$ (CV_8UC3), storages contours and shapes names in 
$processedContours$ and $figures$
*/
void detectFigure(cv::Mat & mask, std::vector<std::vector<cv::Point>>& processedContours, std::vector<std::string>& figures);

/*@brief

Return all nested contours based on vector<Vec4i> hierarchy
*/
std::vector<std::vector<cv::Point>> getChildren(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int item, std::vector<int>& childrensIndex);

/*@brief

Return approximated shape name and writes its contours to $approxContour
*/
std::string checkFigure(std::vector<cv::Point> contour, cv::Mat& draw, std::vector<cv::Point>& approxContour);

/*@brief

Calculates ellipse square (CV_PI*a*b);
*/
float ellipseSquare(cv::RotatedRect& el);