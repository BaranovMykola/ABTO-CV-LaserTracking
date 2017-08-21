#pragma once
#include <opencv2\core.hpp>

#include <vector>
#include <string>

#include "Line.h"

/*@brief

Draw shape based on contours with specific color defined at LaserConsts.h
*/
void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat & mat, int item, int thinknes);

/*@brief

Provide GUI for figures viewing
*/
void mouseCallBack(int event, int x, int y, int flags, void * data);

/*@brief

Return string with line formula y=ax+b
*/
std::string callLine(Line item);

/*@brief

Returns string with signed number +\-numb
*/
std::string signedNumber(int numb);

/*@brief

Returns float rounded to 2 digits after point
*/
std::string roundFloat(float numb, int precision);

/*@brief

Returns point from line edge on distance 5px rotated rotated at the same angle
*/
cv::Point getVolumePoint(cv::Point corner, Line line);