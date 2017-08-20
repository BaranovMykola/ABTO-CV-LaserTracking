#include "DigitDetection.h"

void digitDetection(Mat & digit)
{
	{
		cv::Ptr<cv::ml::SVM> svm = cv::ml::StatModel::load<cv::ml::SVM>("classifier.yml");

		// stats information
		int totalClassifications = 0;
		int totalCorrect = 0;
		int totalWrong = 0;

		// loop over test filenames
		// read image file (grayscale)
		//cv::Mat imgMat = cv::imread(testFilenames[index], 0);

		// convert 2d to 1d	
		Mat imgMat = Mat::zeros(digit.size(), CV_8UC1);
		cvtColor(digit, imgMat, CV_BGR2GRAY);

		Mat points;
		findNonZero(imgMat, points);
		Rect area = boundingRect(points);
		int xEps = area.width*0.1;
		int yEps = area.height*0.1;
		area.x -= xEps;
		area.width += xEps;
		area.y -= yEps;
		area.height += yEps;
		imgMat = imgMat(area);
		Mat resized;
		threshold(imgMat, imgMat, 50, 255, THRESH_BINARY);
		resize(imgMat, resized, Size(28, 28));


		cv::Mat testMat = resized.clone().reshape(1, 1);
		testMat.convertTo(testMat, CV_32F);

		// try to predict which number has been drawn
		try
		{
			int predicted = svm->predict(testMat);
			//std::cout<< "expected: " << expectedLabels[index] << " -> predicted: " << predicted << std::endl;

			// stats						
			cv::imshow("test", resized);
			std::cout << predicted << std::endl;
			cv::waitKey();

		}
		catch (cv::Exception ex)
		{

		}

	}
}
