#pragma once
#include "Plus.h"
#include "trainedBlock.h"

class Bracket :	public trainedBlock
{
public:
	Bracket();
	~Bracket();
	Bracket(const trainedBlock &b);

	void setPos(string _pos);
	string getPos();
	void setOtherBracket(Bracket* other);
	Bracket* getOtherBracket();
	void setCycles(int number);
	int getCycles();
	void setRange(cv::Rect _range);
	cv::Rect getRange();

	void findMatch(vector<Bracket*> tracked);
	void calculateRange(cv::Mat cameraFeed);

	void findPlusBlocks(cv::Mat threshold_plus, cv::Mat cameraFeed);

	void drawBracket(cv::Mat cameraFeed);

private:
	string pos;
	Bracket* otherBracket;
	int cycleNumber;
	cv::Rect range;

	
};



