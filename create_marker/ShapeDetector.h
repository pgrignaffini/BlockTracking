#pragma once

#include <opencv2\imgproc.hpp>
#include "Block.h"

class ShapeDetector
{
	public:

	ShapeDetector();
	~ShapeDetector();

	string detectType(std::vector<std::vector<cv::Point>>& contours, int i);

	std::vector<Block> getTrackedBlocks();
	void setTrackedBlocks(std::vector<Block> identified);

	double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

	private:

	std::vector<Block> trackedBlocks;
};

/*vector<Block> trackFilteredObject(Block& aBlock, Mat threshold, Mat HSV, Mat &cameraFeed, MarkerDetector detector, vector<vector<Point2f>> markerCorners)
{
	Mat temp, canny_out;
	string type;
	int id;
	vector<Block> trackedBlocks;
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;
	ShapeDetector shaper;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	//Canny(temp, canny_out, thresh, thresh * 2, 3);
	Canny(temp, canny_out, 0, 50, 5);
	//find contours of filtered image using openCV findContours function
	findContours(canny_out, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//imshow("Canny", canny_out);

	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		Scalar blue(0, 255, 255);
		drawContours(canny_out, contours, idx, blue, FILLED, 8, hierarchy);
	}


	//imshow("Canny_filled", canny_out);

	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size();
		//cout << "Objects detected: " << numObjects << endl;
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS)
		{
			int index = 0;
			for (; index >= 0; index = hierarchy[index][0])
			{

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.

				if (area > MIN_OBJECT_AREA)
				{
					try
					{
						aBlock.setXPos(moment.m10 / area);
						aBlock.setYPos(moment.m01 / area);
						type = shaper.detectType(contours, index);
						aBlock.setType(type);
						id = detector.findId(aBlock.getXPos(), aBlock.getYPos(), markerCorners);
						aBlock.setID(id);
						trackedBlocks.push_back(aBlock);
						objectFound = true;
					}
					catch (exception& e)
					{
						std::cout << "Something went wrong: " << e.what() << '\n';
					}
				}

				else objectFound = false;
			}
		}
	}

		/// Draw contours
		Mat drawing = Mat::zeros(canny_out.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}

		/// Show in a window
		namedWindow("Contours", WINDOW_AUTOSIZE);
		imshow("Contours", drawing);

		//let user know you found an object
		if (objectFound == true)
		{
			//draw object location on screen
			drawObject(trackedBlocks, cameraFeed);
		}

		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

		return trackedBlocks;

}*/

