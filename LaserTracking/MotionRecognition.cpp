#include "MotionRecognition.h"

#include <opencv2\imgproc.hpp>

using namespace cv;

cv::Mat backgroundSubstract(cv::Mat & frame)
{
	Mat filtered;
	GaussianBlur(frame, filtered, Size(5, 5), 5);

	Mat mask;
	pMOG->apply(filtered, mask);
	return mask;
}
