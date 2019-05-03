#include "stdafx.h"
#include "Hand.h"

#include <stdexcept>

const double MIN_AREA = 3000.0;
const double TIP_THRESH = 0.5;

Hand::Hand()
{
	subtractor = cv::createBackgroundSubtractorMOG2(250, 20, true);
	hsvMin = cv::Scalar(60, 170, 0);
	hsvMax = cv::Scalar(256, 256, 256);
	boundingBox = cv::Rect();
	detected = false;
	fingertip = cv::Point(0, 0);
	imageMat = cv::Mat();
	direction = 0;
	tipArea = cv::Rect();
}

void Hand::setHSV(cv::Scalar min, cv::Scalar max)
{
	hsvMin = min;
	hsvMax = max;
}

void Hand::setInputMat(cv::Mat input)
{
	imageMat = input;
}

void Hand::detect(int method)
{
	switch (method)
	{
	case 0: break;
	case 1:
		hsvThreshold();
		break;
	case 2:
		backgroundSub();
		break;
	default:
		throw std::invalid_argument("invalid detection method specified");
		break;
	}
}

void Hand::calculateTipArea()
{
	int x_offset, y_offset;
	cv::Rect area = cv::Rect();
	int width, height;

	if (fingertip != cv::Point(0, 0) && isDetected())
	{
		if (getDirection() == 1)
		{
			width = BLOCK_WIDTH;
			height = BLOCK_HEIGHT * 4;
			x_offset = fingertip.x - width / 2;
			y_offset = fingertip.y - height / 2;
			area = cv::Rect(x_offset, y_offset, width, height);
		}

		if (getDirection() == 2)
		{
			width = BLOCK_WIDTH * 4;
			height = BLOCK_HEIGHT;
			x_offset = fingertip.x - width / 2;
			y_offset = fingertip.y - height / 2;
			area = cv::Rect(x_offset, y_offset, width, height);
			
		}
	}

	tipArea = area;

}

cv::Scalar Hand::getHSVmin()
{
	return Hand::hsvMin;
}

cv::Scalar Hand::getHSVmax()
{
	return Hand::hsvMax;
}

cv::Point Hand::findFinger(cv::Mat& binaryImage)
{
	std::vector<std::vector<cv::Point>> contours;

	for (int i = 0; i < 3; i++) erode(binaryImage, binaryImage, cv::Mat());
	for (int i = 0; i < 2; i++) dilate(binaryImage, binaryImage, cv::Mat());

	cv::findContours(binaryImage, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	double max_area = 0;
	int contour_index = -1;
	cv::Rect bounding_rect;
	std::vector<std::vector<cv::Point>> hull(contours.size());
	std::vector<std::vector<int>> hullsIndex(contours.size());
	std::vector<std::vector<cv::Vec4i>> defects(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area >= max_area && area > MIN_AREA)
		{
			max_area = area;
			contour_index = i;
			if (contour_index >= 0)
			{
				convexHull(cv::Mat(contours[i]), hull[i]);
				convexHull(contours[i], hullsIndex[i], false);
				bounding_rect = boundingRect(contours[i]);
				boundingBox = bounding_rect;
			}
			if (hullsIndex[i].size() > 3)
			{
				convexityDefects(contours[i], hullsIndex[i], defects[i]);
			}
		}
	}

	std::vector<cv::Point> fingerPoints;
	std::vector<cv::Point> gapPoints;
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < defects[i].size(); j++)
		{
			if (defects[i][j][3] > 13 * 256)
			{
				int fingerIndex = defects[i][j][1];
				int gapIndex = defects[i][j][2];
				cv::Point fingerPoint(contours[i][fingerIndex]);
				cv::Point gapPoint(contours[i][gapIndex]);
				if (bounding_rect.contains(fingerPoint)) fingerPoints.push_back(fingerPoint);
				if (bounding_rect.contains(gapPoint)) gapPoints.push_back(gapPoint);
			}
		}
	}
	if (bounding_rect.width >= bounding_rect.height)
	{
		direction = 2;
		double leftX = bounding_rect.tl().x;
		double rightX = bounding_rect.br().x;
		std::vector<cv::Point> lefts;
		std::vector<cv::Point> rights;
		int fingerIndex = -1;
		for (int i = 0; i < fingerPoints.size(); i++)
		{
			if (abs(leftX - fingerPoints[i].x) <= (TIP_THRESH * bounding_rect.width))
			{
				lefts.push_back(fingerPoints[i]);
			}
			if (abs(rightX - fingerPoints[i].x) <= (TIP_THRESH * bounding_rect.width))
			{
				rights.push_back(fingerPoints[i]);
			}
		}
		if (lefts.size() == 1)
		{
			fingertip = lefts[0];
			detected = true;
		}
		else if (rights.size() == 1)
		{
			fingertip = rights[0];
			detected = true;
		}
		else
		{
			detected = false;
			direction = 0;
		}
	}
	if (bounding_rect.width < bounding_rect.height)
	{
		direction = 1;
		double topY = bounding_rect.tl().y;
		double bottomY = bounding_rect.br().y;
		std::vector<cv::Point> tops;
		std::vector<cv::Point> bottoms;
		int fingerindex = -1;
		for (int i = 0; i < fingerPoints.size(); i++)
		{
			if (abs(topY - fingerPoints[i].y) <= (.05 * bounding_rect.height))
			{
				tops.push_back(fingerPoints[i]);
			}
			if (abs(bottomY - fingerPoints[i].y) <= (.05 * bounding_rect.height))
			{
				bottoms.push_back(fingerPoints[i]);
			}
		}
		if (tops.size() == 1)
		{
			fingertip = tops[0];
			detected = true;
		}
		else if (bottoms.size() == 1)
		{
			fingertip = bottoms[0];
			detected = true;
		}
		else
		{
			detected = false;
			direction = 0;
		}
	}
	return fingertip;
}

cv::Point Hand::getFinger()
{
	return fingertip;
}

void Hand::hsvThreshold()
{
	cv::Mat HSV, threshold;
	cv::cvtColor(imageMat, HSV, cv::COLOR_BGR2HSV);
	cv::inRange(HSV, hsvMin, hsvMax, threshold);
	for (int i = 0; i < 2; i++) erode(threshold, threshold, cv::Mat());
	for (int i = 0; i < 2; i++) dilate(threshold, threshold, cv::Mat());
	
}

void Hand::backgroundSub()
{
	cv::Mat returnMat;
	subtractor->apply(imageMat, returnMat);
	for (int i = 0; i < 2; i++) erode(returnMat, returnMat, cv::Mat());
	for (int i = 0; i < 2; i++) dilate(returnMat, returnMat, cv::Mat());
}

int Hand::getDirection()
{
	return direction;
}

cv::Rect Hand::getBoundingBox()
{
	return boundingBox;
}

bool Hand::isDetected()
{
	return detected;
}

cv::Rect Hand::getTipArea()
{
	return Hand::tipArea;
}
