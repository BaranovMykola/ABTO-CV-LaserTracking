#pragma once
#include <opencv2\core.hpp>

#include <vector>
#include <string>

/*@brief

Draw shape based on contours with specific color defined at LaserConsts.h
*/
void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat & mat, int item, int thinknes);

/*@brief

Provide GUI for figures viewing
*/
void mouseCallBack(int event, int x, int y, int flags, void * data);
