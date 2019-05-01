#pragma once
#include "stdafx.h"
#include "Function.h"

Function::Function() : trainedBlock()
{
	range = cv::Rect2f();
}

Function::~Function()
{

}

Function::Function(const trainedBlock &b) : trainedBlock(b)
{
	range = cv::Rect2f();
}

set<trainedBlock*, xDecr> Function::getBlocks() const
{
	return Function::blocks;
}


cv::Rect2f Function::getRange()
{
	return Function::range;
}

void Function::setBlocks(set<trainedBlock*, xDecr> _blocks)
{
	Function::blocks = _blocks;
}

void Function::setRange(cv::Rect2f _range)
{
	Function::range = _range;
}

int Function::play(int channel)
{
	cout << "Function " << getID() << " plays" << endl;
	
	//mantain a local copy for data consistency
	vector<trainedBlock*> toPlay;
	std::copy(blocks.begin(), blocks.end(), std::back_inserter(toPlay));

	do
	{
		if (!toPlay.empty())
		{
			for (auto it : toPlay)
			{
				try
				{
					it->play(channel);
					do {} while (Mix_Playing(channel)); //wait for the sound to finish playing before playing the next one
				}
				catch (...)
				{
					std::cout << "Exception thrown while playing" << std::endl;
					continue;
				}
			}

		}

	} while (*looping);
	
	return channel;
}

cv::Rect2f Function::findRange(cv::Point2f br)
{
	int x = getXPos();
	int y = getYPos();

	cv::Point2f top_left = cv::Point2f(x + 10, y + BLOCK_HEIGHT);
	cv::Point2f bottom_right = cv::Point2f(br.x, y - BLOCK_HEIGHT);

	cv::Rect2f range = cv::Rect2f(top_left, bottom_right);

	setRange(range);
	return range;
}

void Function::printRange(cv::Mat cameraFeed)
{
	cv::rectangle(cameraFeed, getRange(), cv::Scalar(0, 255, 0));
}

void Function::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*> tblocks)
{
	if (isAReference()) return;

	cv::Rect range = findRange(br);
	cv::Point block_center;

	blocks.clear(); //avoid dangling pointers

	for (auto it : tblocks)
	{
		trainedBlock* current = it.second;
		block_center = cv::Point(current->getXPos(), current->getYPos());

		if (range.contains(block_center))
		{
			blocks.insert(current);
		}
	}

	cout << "Function: " << getID() << " contains ";
	for (auto it : blocks) cout << it->getID() << " ";
	cout << endl;
}



