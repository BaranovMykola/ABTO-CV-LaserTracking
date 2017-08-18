#include "KnnTraining.h"

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\imgproc.hpp>

#include <iostream>
#include <fstream>

#include "LaserDetection.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

KnnTraining::KnnTraining(std::string _fileName, int _label) :
	fileName(_fileName),
	label(_label)
{
}

KnnTraining::~KnnTraining()
{
	ofstream file;
	file.open(fileName, std::ios::app);
	while (!setOfSamples.empty())
	{
		for (auto i : setOfSamples.back())
		{
			file << i << " ";
		}
		file << label << "\n";
		setOfSamples.pop_back();
	}
}

void KnnTraining::startDrawSample(int camera)
{
	int h = 182;
	int s = 121;
	int v = 229;
	int l = 203;
	VideoCapture cap(camera);
	Mat frame;
	int ch;
	cap >> frame;
	LaserTracing trace(frame.size(), 6);

	do
	{
		cap >> frame;
		Mat mask = colorSpaceLaserDetection(frame, h, s, v, l);
		trace.draw(frame, mask);
		
		imshow("frame", frame);
		imshow("trace", trace.getTrace());
		ch = waitKey(30);

		if (ch == 32)
		{
			try
			{
				Mat croppedSample = cropSample(trace.getTrace());
				cout << "Parametrs of cropped sample: Width " << croppedSample.cols << " | Height " << croppedSample.rows << endl;
				imshow("croppedSample", croppedSample);
				cout << "Press esc to reject this image" << endl;
				int act = waitKey();
				if (act == 27)
				{
					trace.clear();
					continue;
				}
				else if (act == 80 || act == 112)
				{
					auto vec = convertToVectorSampple(croppedSample);
					Mat sampleRedict = vectorToMat(vec, CV_32FC1);
					Mat response(0, 0, CV_32FC1);
					float pred = network->findNearest(sampleRedict, network->getDefaultK(), response);
					cout << response << "\t" << pred << endl;
				}
				else
				{
					sample vectorImg = convertToVectorSampple(croppedSample);
					setOfSamples.push_back(vectorImg);
				}
				destroyWindow("croppedSample");

			}
			catch (const std::exception& err)
			{
				cout << "Error handled: " << err.what() << endl;
			}
			trace.clear();
		}

	} 
	while (ch != 27);
}

cv::Ptr<cv::ml::KNearest> KnnTraining::train()
{
	ifstream file;
	vector<sample> trainData;
	vector<int> trainLabel;
	file.open(fileName);
	while (!file.eof())
	{
		int coord;
		sample singleSample;
		for (size_t i = 0; i < 28*28; i++)
		{
			file >> coord;
			singleSample.push_back(coord);
		}
		trainData.push_back(singleSample);
		file >> coord;
		trainLabel.push_back(coord);
	}
	Mat matTrainData = vectorToMat(trainData, CV_32FC1);
	Mat matTrainLabel = vectorToMat(trainLabel, CV_32SC1);

	Ptr<TrainData> trainingData;
	trainingData = TrainData::create(matTrainData, SampleTypes::ROW_SAMPLE, matTrainLabel);

	// learn classifier
	Ptr<KNearest> knn = KNearest::create();
	knn->setIsClassifier(true);
	knn->setAlgorithmType(KNearest::Types::BRUTE_FORCE);
	knn->setDefaultK(1);

	knn->train(trainingData);
	network = knn;
	return knn;
}

cv::Mat KnnTraining::cropSample(cv::Mat & mask) const
{
	Mat planes[3];
	split(mask, planes);
	Mat maskGray = planes[2];
	//cvtColor(mask, maskGray, CV_BGR2GRAY);
	Mat points;
	findNonZero(maskGray, points);
	Rect area = boundingRect(points);
	int x = area.x;
	int y = area.y;
	int width = area.width;
	int height = area.height;

	int diff = abs(width - height);
	if (width > height)
	{
		y -= diff / 2;
		height = width;
	}
	else
	{
		x -= diff / 2;
		width = height;
	}

	if (!
		(x > 0 && x < mask.cols &&
		y > 0 && y < mask.rows &&
		x + width < mask.cols &&
		y + height < mask.rows)
		)
	{
		throw exception("So big sample");
	}
		Rect square(x, y, width, height);
		return maskGray(square);
}

sample KnnTraining::convertToVectorSampple(cv::Mat & cropped) const
{
	Mat resized;
	resize(cropped, resized, Size(28, 28));
	/*namedWindow("resized", CV_WINDOW_FREERATIO);
	imshow("resized", resized);
	waitKey();
	destroyWindow("resized");*/

	sample currentImg;
	for (size_t i = 0; i < resized.rows; i++)
	{
		uchar* row = resized.ptr<uchar>(i);
		currentImg.insert(currentImg.end(), row, row + 28);
	}


	return currentImg;
}

Mat KnnTraining::vectorToMat(vector<vector<int> > samplesVec, int type)
{
	Mat trainDataMat(samplesVec.size(), samplesVec.back().size(), type);
	for (size_t i = 0; i < trainDataMat.rows; i++)
	{
		float* row = trainDataMat.ptr<float>(i);
		for (size_t j = 0; j < trainDataMat.cols; j++)
		{
			row[j] = samplesVec[i][j];

		}
	}
	return trainDataMat;
}

Mat KnnTraining::vectorToMat(vector<int> labelsVec, int type)
{
	Mat labelsMat(1, labelsVec.size(), type);
	for (size_t i = 0; i < labelsMat.rows; i++)
	{
		int* row = labelsMat.ptr<int>(i);
		for (size_t j = 0; j < labelsMat.cols; j++)
		{
			row[j] = labelsVec[j];
		}
	}
	return labelsMat;
}