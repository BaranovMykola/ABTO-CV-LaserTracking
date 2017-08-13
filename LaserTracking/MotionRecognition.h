#pragma once
#include <opencv2\core.hpp>
#include <opencv2\video.hpp>

static cv::Ptr<cv::BackgroundSubtractor> pMOG = cv::createBackgroundSubtractorMOG2();

cv::Mat backgroundSubstract(cv::Mat& frame);