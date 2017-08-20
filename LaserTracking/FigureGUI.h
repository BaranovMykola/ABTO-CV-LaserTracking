#pragma once
#include <opencv2\core.hpp>

#include <vector>
#include <string>

void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat & mat, int item, int thinknes);

void mouseCallBack(int event, int x, int y, int flags, void * data);
