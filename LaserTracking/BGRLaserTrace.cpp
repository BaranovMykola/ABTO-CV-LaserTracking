#include "BGRLaserTrace.h"

#include <opencv2\imgproc.hpp>

#include "LaserDetection.h"

using namespace cv;

cv::Mat checkRedLaser(cv::Mat & mask, cv::Mat & frame)
{
	Mat laser;
	Mat points;
	cv::Rect laserArea;
	findMinimumMotionArea(mask, laserArea);
	laser = frame(laserArea);
	Scalar average = mean(laser);
	float s = laser.rows*laser.cols;
	float exc = points.rows;
	if (!(average[2] >= average[1] &&
		  average[2] >= average[0])) // 5%
	{
		mask = 0;
	}
	return mask;
}