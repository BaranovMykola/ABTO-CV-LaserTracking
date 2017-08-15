#pragma once
#include <opencv2\core.hpp>
#include <vector>

class ProjectorCalibration
{
public:
	ProjectorCalibration();
	~ProjectorCalibration();

	void drawControlPoints(int camera);
private:
	std::vector<cv::Point> LaserPoints;
	std::vector<cv::Point> ProjectorPoints;
	const static int pointsCountCalibration = 3;
};

