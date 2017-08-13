#include "HSVLaserTrace.h"
#include <opencv2\imgproc.hpp>

using namespace cv;
using namespace std;

cv::Mat hsvLaserDetect(cv::Mat & frame, int hThresh, int sThresh, int vThresh)
{
	Mat frameHSV;
	cvtColor(frame, frameHSV, CV_BGR2HSV);
	Mat hsvPlanes[3];
	split(frameHSV, hsvPlanes);

	threshold(hsvPlanes[2], hsvPlanes[2], hThresh, 255, CV_THRESH_TOZERO);
	threshold(hsvPlanes[1], hsvPlanes[1], sThresh, 255, CV_THRESH_BINARY_INV);
	threshold(hsvPlanes[0], hsvPlanes[0], vThresh, 255, CV_THRESH_BINARY_INV);

	Mat res(frame.size(), CV_8UC1);
	bitwise_and(hsvPlanes[0], hsvPlanes[1], res);
	bitwise_and(res, hsvPlanes[2], res);
	return res;
}
