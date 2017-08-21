#include "FigureGUI.h"

#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>

#include <iomanip>

#include "LaserConsts.h"

using namespace cv;
using namespace std;

void drawColorShape(std::vector<std::vector<cv::Point>> contours, std::vector<std::string> figures, cv::Mat& mat, int item, int thinknes)
{
	auto it = shapeColors.find(figures[item]);
	Scalar color;
	if (it == shapeColors.end())
	{
		color = Scalar(0,239,255);
	}
	else
	{
		color = it->second;
	}
	drawContours(mat, contours, item, color, thinknes);
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
			putText(mat, figures[i], Point(x,y), cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar::all(255), 1);
			drawColorShape(contours, figures, mat, i, 5);
		}
	}
	imshow("Detected figures", mat);
}

std::string callLine(Line item)
{
	string name = "y=";
	if (item.vertical)
	{
		name = "x="+std::to_string((int)item.a);
		return name;
	}
	string a = roundFloat(item.a, 2);
	string b = roundFloat(item.b, 2);
	if (item.a != 0)
	{
		if(a != "0")
			name += a+"x";

	}
	if (item.b != 0)
	{
		if (b[0] != '-' && a != "0")
			name += "+";
		name += b;
	}
	return name;
}

std::string signedNumber(int numb)
{
	string sign = numb > 0 ? "+" : "";
	string number = std::to_string(numb);
	if (numb == 1)
	{
		return string();
	}
	if (numb == -1)
	{
		return "-";
	}
	return sign + number;
}

string roundFloat(float numb, int precision)
{
	numb *= 10 * precision;
	numb = (int)numb;
	numb /= 10 * precision;
	int numbInt = numb;
	int decimalCount = std::to_string(numbInt).length();
	std::ostringstream str;
	str << setprecision(decimalCount + precision) << numb;
	string res = str.str();
	return res;
}

cv::Point getVolumePoint(cv::Point corner, Line line)
{
	string as = roundFloat(line.a, 2);
	if (as != "0")
	{
		//return corner + Point(5, 0);
	}
	int x0 = corner.x;
	int y0 = corner.y;
	float newAngle = line.angle() + 90;
	float newAngleRad = newAngle*(CV_PI/180);
	float a = std::tan(newAngleRad);
	float b = -a*x0 + y0;

	float A = a*a + 1;
	float B = -2 * x0 + 2 * a*b - 2 * a*y0;
	float C = x0*x0 + b*b - 2 * b*y0 + y0*y0;

	float D1 = B*B - 4 * A*(C + 25);

	float x1 = (-B + sqrt(D1)) / (2 * A);
	float x2 = (-B  -sqrt(D1)) / (2 * A);

	float D2 = B*B - 4 * A*(C - 25);

 	float x3 = (-B + sqrt(D2)) / (2 * A);
	float x4 = (-B - sqrt(D2)) / (2 * A);

	float y1 = a*x1 + b;
	float y2 = a*x2 + b;
	float y3 = a*x3 + b;
	float y4 = a*x4 + b;
	if(D1 > 0)
		return Point(x1, y1);
	if (D2 > 0)
		return Point(x3, y3);
}
