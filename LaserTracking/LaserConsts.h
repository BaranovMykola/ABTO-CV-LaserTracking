#pragma once
#include <opencv2\core.hpp>
#include <opencv2\video.hpp>

#include <string>
#include <map>

const std::string figuresName[9] = { "Triangle", "Quadrangle", "Pentagon", "Hexagon", "Heptagon", "Octagon", "Nonagon", "Rectangle", "Square" };
const cv::Scalar figureColor(0, 255, 0);
const cv::Scalar ellipseColor(255, 0, 0);
const cv::Scalar circleColor(255, 255, 0);
const cv::Scalar textColor(255, 255, 255);

const std::map<std::string, cv::Scalar> shapeColors =
{
	std::make_pair("Triangle", figureColor),
	std::make_pair("Rectangle", cv::Scalar(0,213,255)),
	std::make_pair("Pentagon", cv::Scalar(213,255,0)),
	std::make_pair("Hexagon", cv::Scalar(255,142,55)),
	std::make_pair("Heptagon", cv::Scalar(239,25,225)),
	std::make_pair("Octagon", cv::Scalar(102,51,153)),
	std::make_pair("Nonagon", figureColor),
	std::make_pair("Circle", circleColor),
	std::make_pair("Ellipse", ellipseColor),
	std::make_pair("Quadrangle", circleColor),
	std::make_pair("Square", ellipseColor)
};

static cv::Rect default;

static cv::Ptr<cv::BackgroundSubtractor> pMOG = cv::createBackgroundSubtractorMOG2();