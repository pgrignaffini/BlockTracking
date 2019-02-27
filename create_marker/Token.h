#pragma once
class Token
{
public:
	Token();
	~Token();

	cv::Scalar getHSVmin();
	cv::Scalar getHSVmax();

private:
	cv::Scalar HSVmin;
	cv::Scalar HSVmax;
};

