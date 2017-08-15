#include "LineDetector.h"

#include <opencv2\imgproc.hpp>

using namespace std;
using namespace cv;

void detectLine(cv::Mat & frame, cv::Mat& mask)
{
	morphologyEx(mask, mask, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(6, 6)));
	vector<Vec2f> lines;
	HoughLines(mask, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(frame, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
}
