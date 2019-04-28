#ifndef HAND_H
#define HAND_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <cmath>
#include <iostream>
#include <string>

class Hand
{
	int direction;
	cv::Point fingertip;
	bool detected;
	cv::Scalar hsvMin;
	cv::Scalar hsvMax;
	cv::Rect boundingBox;
	cv::Mat imageMat;
	cv::Ptr<cv::BackgroundSubtractor> subtractor;
	cv::Rect tipArea;

	void hsvThreshold();
	void backgroundSub();

public:
	Hand();

	void setHSV(cv::Scalar min, cv::Scalar max);
	void setInputMat(cv::Mat input);
	void detect(int method = 1);
	void calculateTipArea();

	cv::Scalar getHSVmin();
	cv::Scalar getHSVmax();
	int getDirection();
	cv::Point findFinger(cv::Mat& binaryImage);
	cv::Point getFinger();
	cv::Rect getBoundingBox();
	bool isDetected();
	cv::Rect getTipArea();
};

#endif