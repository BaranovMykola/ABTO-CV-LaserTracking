#pragma once
#include <opencv2/core.hpp>
#include <string>


std::string detectFigure(cv::Mat& mask);

void trainDataConsume(cv::Mat & mask);

void showTrainData();
