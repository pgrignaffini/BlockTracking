#pragma once
#include "stdafx.h"
#include "ColorCalibration.h"

ColorCalibration::ColorCalibration()
{
	H_MIN = 0;
	H_MAX = 256;
	S_MIN = 0;
	S_MAX = 256;
	V_MIN = 0;
	V_MAX = 256;
}


ColorCalibration::~ColorCalibration()
{
}

void ColorCalibration::on_trackbar(int, void*)
{
	//This function gets called whenever a
	// trackbar position is changed
}

void ColorCalibration::createTrackbars()
{
	//create window for trackbars
	cv::namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf_s(TrackbarName, "H_MIN", H_MIN);
	sprintf_s(TrackbarName, "H_MAX", H_MAX);
	sprintf_s(TrackbarName, "S_MIN", S_MIN);
	sprintf_s(TrackbarName, "S_MAX", S_MAX);
	sprintf_s(TrackbarName, "V_MIN", V_MIN);
	sprintf_s(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	cv::createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, &ColorCalibration::on_trackbar);
	cv::createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, &ColorCalibration::on_trackbar);
	cv::createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, &ColorCalibration::on_trackbar);
	cv::createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, &ColorCalibration::on_trackbar);
	cv::createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, &ColorCalibration::on_trackbar);
	cv::createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, &ColorCalibration::on_trackbar);
}
