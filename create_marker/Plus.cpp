#include "stdafx.h"
#include "Plus.h"


Plus::Plus()
{
	setHSVmin(cv::Scalar(0, 0, 188));//hardcoded red color
	setHSVmax(cv::Scalar(7, 256, 224));
}


Plus::~Plus()
{
}
