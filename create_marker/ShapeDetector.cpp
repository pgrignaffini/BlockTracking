#include "stdafx.h"
#include "ShapeDetector.h"
#include <iostream>


ShapeDetector::ShapeDetector(){}

ShapeDetector::~ShapeDetector(){}

string ShapeDetector::detectType(std::vector<std::vector<cv::Point>>& contours, int i)
{
	string block_type;
	vector<cv::Point> approx;

	approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);
		//cout << "Vertex" << nvertex << endl;
		//std::cout << "Block size: " << blocks.size();

	if (approx.size() >= 4)
	{
		// Number of vertices of polygonal curve
		int vtc = approx.size();

		// Get the cosines of all corners
		vector<double> cos;
		for (int j = 2; j < vtc + 1; j++)
			cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));
		
		// Sort ascending the cosine values
		sort(cos.begin(), cos.end());

		// Get the lowest and the highest cosine
		double mincos = cos.front();
		double maxcos = cos.back();

		// Use the degrees obtained above and the number of vertices
		// to determine the shape of the contour
		if (approx.size() == 4 && mincos >= -0.1 && maxcos <= 0.3)
		{
			cv::Rect r = boundingRect(contours[i]);
			double ratio = abs(1 - (double)r.width / r.height);

			if (ratio <= 0.15)
			{
				block_type = "variable";
			}

			else
			{
				block_type = "break";
			}
		}

		else if (approx.size() == 6 && maxcos < 0.3) //noise makes the pentagon cut on the top, so the vertices are 6
		{
			block_type = "function";
		}

		else if (approx.size() >= 8 && maxcos < 0.3)
		{
			double area = contourArea(contours[i]);
			cv::Rect r = boundingRect(contours[i]);
			int radius = r.width / 2;

			if (abs(1 - ((double)r.width / r.height)) <= 0.3 &&
				abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.3)
			{
				block_type = "note";
			}

			else block_type = "bracket";
		}

	}

	return block_type;
}

std::vector<Block> ShapeDetector::getTrackedBlocks()
{
	return ShapeDetector::trackedBlocks;
}

void ShapeDetector::setTrackedBlocks(std::vector<Block> identified)
{
	ShapeDetector::trackedBlocks = identified;
}

double ShapeDetector::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}