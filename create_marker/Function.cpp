#include "stdafx.h"
#include "Function.h"

Function::Function()
{
}


Function::~Function()
{
}


Function::Function(const trainedBlock &b) : trainedBlock(b)
{
	cycles = 0;
	range = cv::Rect2f();
	defined = false;
}


set<trainedBlock*, xDecr>* Function::getBlocks() const
{
	return Function::blocks;
}

int Function::getCycles()
{
	return Function::cycles;
}

cv::Rect2f Function::getRange()
{
	return Function::range;
}

bool Function::isDefined()
{
	return Function::defined;
}

void Function::setBlocks(set<trainedBlock*, xDecr>* _blocks)
{
	Function::blocks = _blocks;
}

void Function::setRange(cv::Rect2f _range)
{
	Function::range = _range;
}

void Function::incrementCycles(int n)
{
	Function::cycles += n;
}

void Function::addBlock(trainedBlock* block)
{
	Function::blocks->insert(block);
}

void Function::reset_cycles()
{
	cycles = 0;
}

void Function::play()
{
	cout << "Function " << getID() << " plays" << endl;
	string type;
	vector<trainedBlock*> toDelete;

	int ncycles = getCycles();

	for (auto it : *blocks)
	{
		type = it->getType();

		if (type == "canc") toDelete.push_back(it);
	}

	for (auto it : toDelete)
	{
		delete it;
		blocks->erase(it);
	}


	for (int j = 0; j < ncycles; j++)
	{
		for (auto it : *blocks)
		{
			it->play();
		}
	}
}

cv::Rect2f Function::findRange(cv::Point2f br)
{
	int x = getXPos();
	int y = getYPos() + BLOCK_HEIGHT / 2;

	cv::Point2f top_left = cv::Point2f(x, y);
	cv::Point2f bottom_right = cv::Point2f(br.x, y - BLOCK_HEIGHT);

	cv::Rect2f range = cv::Rect2f(bottom_right, top_left);

	setRange(range);
	return range;
}

void Function::printRange(cv::Mat cameraFeed)
{
	cv::rectangle(cameraFeed, getRange(), cv::Scalar(0, 255, 0));
}

void Function::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks)
{
	cv::Rect2f range = findRange(br);
	cv::Point2f block_center;

	for (auto it : tblocks)
	{
		trainedBlock* current = it.second;
		block_center = cv::Point2f(current->getXPos(), current->getYPos());

		if (range.contains(block_center))
		{
			addBlock(current);
		}
	}

	cout << "Function: " << getID() << " contains ";
	for (auto it : *blocks) cout << it->getID() << " ";
	cout << endl;
}

void Function::countCycles(cv::Mat threshold_plus)
{
	cv::Mat temp, canny_out;
	cv::Rect range = getRange();

	reset_cycles();
	//cout << "area" << range.area() << endl;

	if (range.area() > 0)
	{
		threshold_plus.copyTo(temp); 
		//these two vectors needed for output of findContours
		vector<vector<cv::Point>> contours;
		vector<cv::Vec4i> hierarchy;

		Canny(temp, canny_out, 0, 50, 5);
		//imshow("Canny", canny_out);


		//find contours of filtered image using openCV findContours function
		findContours(canny_out, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		int index = 0;
		int xPos, yPos;

		for (; index >= 0; index = hierarchy[index][0])
		{
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;

			if (area > 10 * 10)
			{
				xPos = moment.m10 / area;
				yPos = moment.m01 / area;

				cv::Point plusPosition = cv::Point(xPos, yPos);

				//circle(cameraFeed, plusPosition, 10, cv::Scalar(0, 255, 0));

				if (range.contains(plusPosition)) //rect for plus blocks
				{
					incrementCycles(1);
				}
			}

		}
	}

	cout << "Function " << getID() << " contains " << getCycles() << " cycles" << endl;
}


