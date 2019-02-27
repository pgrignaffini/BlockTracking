#pragma once
#include "stdafx.h"

class ColorCalibration
{
public:
	//initial min and max HSV filter values.
	//these will be changed using trackbars
	int H_MIN;
	int H_MAX;
	int S_MIN;
	int S_MAX;
	int V_MIN;
	int V_MAX ;

	const std::string trackbarWindowName = "Trackbars";

	ColorCalibration();
	~ColorCalibration();

	void static on_trackbar(int, void*);
	void createTrackbars();
};

