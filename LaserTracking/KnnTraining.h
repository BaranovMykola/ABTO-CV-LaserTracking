#pragma once
#include <opencv2\core.hpp>
#include <opencv2\ml.hpp>

#include <vector>
#include <string>

#include "LaserTracing.h"

typedef std::vector<int> sample;

class KnnTraining
{
public:
	KnnTraining(std::string _fileName, int _label);
	~KnnTraining();

	void startDrawSample(int camera);
	cv::Ptr<cv::ml::KNearest> train();

private:

	cv::Mat cropSample(cv::Mat& mask)const;
	sample convertToVectorSampple(cv::Mat& cropped)const;

	cv::Mat vectorToMat(std::vector<sample> samplesVec, int type);
	cv::Mat KnnTraining::vectorToMat(std::vector<int> labelsVec, int type);

	int label;
	std::vector<sample> setOfSamples;
	std::string fileName;
	cv::Ptr<cv::ml::KNearest> network;
};

