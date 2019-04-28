#include "stdafx.h"
#include "Token.h"

Token::Token(cv::Point position)
{
	HSVmin = cv::Scalar(0, 0, 0); //black
	HSVmax = cv::Scalar(256, 256, 60); //black 
	pos = position;
}

Token::Token()
{
	HSVmin = cv::Scalar(0, 0, 0); //black
	HSVmax = cv::Scalar(256, 256, 60); //black 
}


Token::~Token()
{
}

cv::Scalar Token::getHSVmin()
{
	return Token::HSVmin;
}

cv::Scalar Token::getHSVmax()
{
	return Token::HSVmax;
}

cv::Point Token::getPos()
{
	return Token::pos;
}

int Token::getXPos()
{
	return Token::pos.x;
}

int Token::getYPos()
{
	return Token::pos.y;
}
