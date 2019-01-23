#pragma once

#include <string>
#include <opencv2\core.hpp>
#include "stdafx.h"

using namespace std;

class Block
{
	public:

	Block();
	virtual ~Block() = default; //polymorphic

	virtual void play();

	Block(string type);
	Block(const Block &_block);

	int getID() const;
	int getXPos() const;
	int getYPos() const;
	string getType() const;
	cv::Scalar getHSVmin() const;
	cv::Scalar getHSVmax() const;

	void setID(int ID);
	void setXPos(int x);
	void setYPos(int y);
	void setType(string type);
	void setHSVmin(cv::Scalar min);
	void setHSVmax(cv::Scalar max);

	private:

	int id;
	int xPos, yPos;
	string type;
	cv::Scalar HSVmin, HSVmax;

};

