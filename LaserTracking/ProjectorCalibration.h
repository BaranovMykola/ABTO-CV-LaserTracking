#pragma once
#include <opencv2\core.hpp>
#include <vector>

#include "LaserTracing.h"

enum Capture
{
	Wait = 0,
	Calibration = 1
};

const int AffinePoints = 3;
const int HomographyPoints = 4;

class ProjectorCalibration
{
public:
	ProjectorCalibration(cv::Size resolution);
	~ProjectorCalibration();

	void drawControlPoints(int camera);

private:
	void pointApproximation(cv::Mat& frame, LaserTracing& trace);
	void projectorDrawPoint(cv::Size frameSize);
	void projectorSearchPoint(cv::Mat& background, cv::Mat& frame);

	std::vector<cv::Point> LaserPoints;
	std::vector<cv::Point> ProjectorPoints;
	int pointsCountCalibration = AffinePoints;
	cv::Size projectorResolution;
	cv::Mat projectorImage;
};

