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
	/*
	@param frameSize Laser trace size 
	*/
	LaserTracing(cv::Size frameSize);
	~LaserTracing();
	
	/*@brief
	
	Calculate laser trace
	
	@param frame Used for getting signle point from laser
	@param mask Nonzero pizels represent laser
	*/
	void draw(cv::Mat& frame, cv::Mat& mask);
	/*@brief

	Returns laser trace in CV_8UC3
	*/
	cv::Mat getTrace()const;
	/*@brief
	
	Erases trace
	*/
	void clear();

private:
	cv::Point getLaserPoint(cv::Mat& frame, cv::Mat& mask);

	cv::Point from;
	cv::Point to;
	cv::Mat trace;
	LaserState state;
	bool drawen;
};

