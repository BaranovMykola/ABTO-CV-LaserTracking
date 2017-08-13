#include "YUVLaserTrace.h"
#include <opencv2\imgproc.hpp>

using namespace cv;

cv::Mat yuvLaserDetect(cv::Mat & frame, int lightThresh)
{
	Mat hslFrame;
	cvtColor(frame, hslFrame, CV_BGR2YUV);
	Mat yuvPlanes[3];
	split(hslFrame, yuvPlanes);
	threshold(yuvPlanes[0], yuvPlanes[0], lightThresh, 255, THRESH_TOZERO);
	return yuvPlanes[0];
}
