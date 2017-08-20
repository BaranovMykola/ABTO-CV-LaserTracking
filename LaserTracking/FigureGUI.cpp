#include "FigureGUI.h"

#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>

#include "LaserConsts.h"

using namespace cv;
using namespace std;

void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat& mat, int item, int thinknes)
{
	auto it = shapeColors.find(figures[item]);
	Scalar color;
	if (it == shapeColors.end())
	{
		color = Scalar::all(255);
	}
	else
	{
		color = it->second;
	}
	drawContours(mat, contours, item, color, 5);
}

void mouseCallBack(int event, int x, int y, int flags, void* data)
{
	auto cont = static_cast<std::tuple<std::vector<std::vector<cv::Point>>*, std::vector<std::string>*, cv::Mat*>*>(data);
	auto contours = *std::get<0>(*cont);
	auto figures = *std::get<1>(*cont);
	auto mat = std::get<2>(*cont)->clone();

	for (int i = 0;i<contours.size();++i)
	{
		if (pointPolygonTest(contours[i], Point(x, y), false) > 0)
		{
			putText(mat, figures[i], contours[i][0], cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255), 1);
			drawColorShape(contours, figures, mat, i, 5);
		}
	}
	imshow("Detected figures", mat);
}