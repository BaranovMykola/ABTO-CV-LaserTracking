#pragma once
#include <opencv2\core.hpp>

enum LaserState
{
	Draw = 0,
	Sleep = 1,
	Notification = 2
};

class LaserTracing
{
public:
	LaserTracing(cv::Size frameSize);
	~LaserTracing();

	void draw(cv::Mat& frame, cv::Mat& mask);
	cv::Point getLaserPoint(cv::Mat& frame, cv::Mat& mask);
	cv::Mat getTrace()const;
	void clear();

private:
	cv::Point from;
	cv::Point to;
	cv::Mat trace;
	LaserState state;
	bool drawen;
};

