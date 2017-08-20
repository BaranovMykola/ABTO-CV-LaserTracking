#pragma once
#include <opencv2\core.hpp>
#include <opencv2\ml.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>

#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::ml;



void digitDetection(Mat& digit);