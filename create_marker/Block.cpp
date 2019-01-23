#include "stdafx.h"
#include "Block.h"


Block::Block()
{
	//if we need to detect a different color we need to change the HSV values accordingly
	
	setHSVmin(cv::Scalar(0, 86, 177));//hardcoded orange color
	setHSVmax(cv::Scalar(65, 256, 256));
	
	//setHSVmin(cv::Scalar(0, 103, 168));//arancione casa
	//setHSVmax(cv::Scalar(32, 256, 256)); //arancione casa
	//setHSVmin(cv::Scalar(0, 72, 188));//arancione unipr
	//setHSVmax(cv::Scalar(75, 256, 256)); //arancione unipr
	setXPos(0);
	setYPos(0);
	setID(-1);
	setType("X");
}

void Block::play()
{
}

Block::Block(string type)
{
	Block::Block();
	setType(type);
}

Block::Block(const Block & _block)
{
	setType(_block.type);
	setID(_block.id);
	setXPos(_block.xPos);
	setYPos(_block.yPos);
	setHSVmin(_block.HSVmin);
	setHSVmax(_block.HSVmax);
}

int Block::getXPos() const
{
	return Block::xPos;
}

int Block::getYPos() const
{
	return Block::yPos;
}

void Block::setXPos(int x)
{
	Block::xPos = x;
}

void Block::setYPos(int y)
{
	Block::yPos = y;
}

cv::Scalar Block::getHSVmin() const
{
	return Block::HSVmin;
}

cv::Scalar Block::getHSVmax() const
{
	return Block::HSVmax;
}

void Block::setHSVmin(cv::Scalar min)
{
	Block::HSVmin = min;
}

void Block::setHSVmax(cv::Scalar max)
{
	Block::HSVmax = max;
}

string Block::getType() const
{
	return Block::type;
}

void Block::setType(string type)
{
	Block::type = type;
}

int Block::getID() const
{
	return Block::id;
}

void Block::setID(int ID)
{
	Block::id = ID;
}

