#pragma once

const int LOOP_AREA = 1500;

class Token
{
public:
	Token(cv::Point position);
	Token();
	~Token();

	cv::Scalar getHSVmin();
	cv::Scalar getHSVmax();
	cv::Point getPos();
	int getXPos();
	int getYPos();

private:
	cv::Scalar HSVmin;
	cv::Scalar HSVmax;
	cv::Point pos;
};

