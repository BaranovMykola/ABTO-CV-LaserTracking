#include "FigureDetection.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\ml.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace cv;
using namespace cv::ml;

static int morphSize = 50;
static int eps = 60;
static int acc = 7;
static int Rk = 1;
static int elispeEps = 10;
static std::string figuresName[5] = { "Triangle", "Rectangle", "Pentagon", "Hexagon", "Circle" };
std::string detectFigure(cv::Mat & mask)
{
	namedWindow("Panel");
	createTrackbar("MorphSize", "Panel", &morphSize, 150);
	createTrackbar("Eps", "Panel", &eps, 150);
	createTrackbar("Acc", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &Rk, 400);
	createTrackbar("elipseEps/10", "Panel", &elispeEps, 30);
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	string figure;
	do
	{
		Mat morph;
		dilate(maskGray, morph, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
		morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));

		vector < vector<Point> > contours;
		findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		drawContours(morph, contours, -1, Scalar(255, 0, 0));
		imshow("morph", morph);
		Mat draw = mask.clone();
		for (auto i : contours)
		{
			Rect area = boundingRect(i);
			eps = std::min(area.height, area.width) / 3;
			acc = eps / 10;
			vector<Point> approx;
			vector<Point> accuracyContour;
			approxPolyDP(i, approx, eps, true);
			approxPolyDP(i, accuracyContour, acc, true);
			drawContours(draw, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 1);
			drawContours(draw, vector<vector<Point>>{accuracyContour}, -1, Scalar(255, 255, 255), 1);
			//cout << "Accuracy contour corners - " << accuracyContour.size() << endl;
			Point2f center;
			float radius;
			minEnclosingCircle(i, center, radius);
			//RotatedRect elipse = fitEllipse(i);

			cout << "R = " << radius << endl;
			int acc = accuracyContour.size();
			int apr = approx.size();
			float lenghtCircle = 2 * CV_PI*radius;
			float lenghtFigure = arcLength(i, true);
			float lenghtRatio = lenghtFigure / lenghtCircle;
			float squareFigure = contourArea(i);
			float squareCircle = CV_PI*radius*radius;
			float squareRatio = squareFigure / squareCircle;

			cout << "FigureS / Circle S = " << squareFigure << "/" << squareCircle << " = " << squareRatio << endl;
				circle(draw, center, radius, Scalar(255, 0, 0), 1);
			if (squareRatio > 0.7  || (apr <= 2 || apr > 6))
			{
				cout << "Found circle " << endl;
				figure = figuresName[4];
			}
			else
			{
				figure = figuresName[approx.size() - 3];
			}
			if (acc - apr >= 10)
			{
				figure = "???";
			}
			cout << "Founded " << approx.size() << " corners " << endl;
		}
		putText(draw, figure, Point(100, 100), FONT_HERSHEY_COMPLEX, 2, Scalar::all(255));
		imshow("figure", draw);
	} while (waitKey(30) != 27);
	destroyWindow("figure");
	destroyWindow("morph");
	destroyWindow("Panel");
	return figure;
}

void trainDataConsume(cv::Mat & mask)
{
	vector<vector<int>> trainData;
	ifstream file("trainData.txt");
	while (!file.eof())
	{
		int x;
		int y;
		int classData;
		file >> x >> y >> classData;
		trainData.push_back(vector<int>{x, y, classData});
	}
	file.close();


	/*namedWindow("Panel");
	createTrackbar("MorphSize", "Panel", &morphSize, 150);
	createTrackbar("Eps", "Panel", &eps, 150);
	createTrackbar("Acc", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &acc, 150);
	createTrackbar("Rk", "Panel", &Rk, 400);
	createTrackbar("elipseEps/10", "Panel", &elispeEps, 30);*/
	Mat maskGray;
	cvtColor(mask, maskGray, CV_BGR2GRAY);
	string figure;

	Mat morph;
	dilate(maskGray, morph, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));
	morphologyEx(maskGray, morph, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(morphSize, morphSize)));

	vector < vector<Point> > contours;
	findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(morph, contours, -1, Scalar(255, 0, 0));
	//imshow("morph", morph);
	Mat draw = mask.clone();
	for (auto i : contours)
	{
		vector<Point> approx;
		vector<Point> accuracyContour;
		approxPolyDP(i, approx, eps, true);
		approxPolyDP(i, accuracyContour, acc, true);
		drawContours(draw, vector<vector<Point>>{approx}, -1, Scalar(0, 255, 0), 1);
		//drawContours(draw, vector<vector<Point>>{accuracyContour}, -1, Scalar(255, 255, 255), 1);
		//cout << "Accuracy contour corners - " << accuracyContour.size() << endl;
		Point2f center;
		float radius;
		minEnclosingCircle(i, center, radius);

		cout << "R = " << radius << endl;
		int acc = accuracyContour.size();
		int apr = approx.size();
		imshow("trace", mask);
		int ch = waitKey();
		int classData;
		if (ch == 32)
		{
			cout << "Acc " << acc << " r " << radius << " accepted " << endl;
			classData = 1;
		}
		else if (ch == 27)
		{
			cout << "Acc " << acc << " r " << radius << " rejected " << endl;
			classData = 0;
		}
		else
		{
			cout << "Rejected by user";
			return;
		}

		trainData.push_back(vector<int>{acc, (int)radius, classData});
	}

	ofstream wr("trainData.txt");
	while (!trainData.empty())
	{
		auto row = trainData.back();
		wr << row[0] << " " << row[1] << " " << row[2] << endl;
		trainData.pop_back();
	}
}

void showTrainData()
{
	ifstream file("trainData.txt");
	Mat trainData(110, 2, CV_32FC1);
	Mat labels(110, 1, CV_32SC1);
	int i = 0;
	Mat img_res = Mat::zeros(Size(30, 300), CV_8UC3);
	while (!file.eof())
	{
		int x;
		int y;
		int classData;
		file >> x >> y >> classData;
		/*trainData.push_back(vector<int>{x, y, classData});*/
		trainData.at<float>(i, 0) = x;
		trainData.at<float>(i, 1) = y;
		labels.at<float>(i, 0) = classData;
		Vec3b color;
		if (classData == 1)
		{
			color = Vec3b(0, 0, 255);
		}
		else
		{
			color = Vec3b(0, 255, 0);
		}
		//img_res.at<Vec3b>(x, y) = color;
		circle(img_res, Point(x, y), 3, color, 1);
	}
	namedWindow("vizual", CV_WINDOW_FREERATIO);
	imshow("vizual", img_res);

	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::KernelTypes::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	svm->train(trainData, ROW_SAMPLE, labels);

	//------------------------ 4. Show the decision regions ----------------------------------------
	Vec3b green(0, 100, 0), blue(100, 0, 0);
	for (int i = 0; i < img_res.rows; ++i)
		for (int j = 0; j < img_res.cols; ++j)
		{
			Mat sampleMat = (Mat_<float>(1, 2) << i, j);
			float response = svm->predict(sampleMat);

			if (response == 1)    img_res.at<Vec3b>(j, i) = green;
			else if (response == 2)    img_res.at<Vec3b>(j, i) = blue;
		}

	//----------------------- 5. Show the training data --------------------------------------------
	int thick = -1;
	int lineType = 8;
	float px, py;
	// Class 1
	//for (int i = 0; i < NTRAINING_SAMPLES; ++i)
	//{
	//	px = trainData.at<float>(i, 0);
	//	py = trainData.at<float>(i, 1);
	//	circle(img_res, Point((int)px, (int)py), 3, Scalar(0, 255, 0), thick, lineType);
	//}
	//// Class 2
	//for (int i = NTRAINING_SAMPLES; i <2 * NTRAINING_SAMPLES; ++i)
	//{
	//	px = trainData.at<float>(i, 0);
	//	py = trainData.at<float>(i, 1);
	//	circle(img_res, Point((int)px, (int)py), 3, Scalar(255, 0, 0), thick, lineType);
	//}

	//------------------------- 6. Show support vectors --------------------------------------------
	thick = 2;
	lineType = 8;
	Mat sv = svm->getSupportVectors();

	for (int i = 0; i < sv.rows; ++i)
	{
		const float* v = sv.ptr<float>(i);
		circle(img_res, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thick, lineType);
	}

	//imwrite("result.png", I);                      // save the Image
	imshow("SVM", img_res); // show it to the user
	waitKey(0);

}
