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

void Function::addBlock(trainedBlock* block)
{
	Function::blocks.insert(block);
}

bool Function::firstInLine()
{
	return false;
}

int Function::play(int channel)
{
	cout << "Function " << getID() << " plays" << endl;
	//cout << "Function " << getID() << " contains " << *getCycles() << " cycles" << endl;
	string type;
	vector<trainedBlock*> toDelete;

	//int ncycles = *getCycles();

	vector<trainedBlock*> toPlay;
	std::copy(blocks.begin(), blocks.end(), std::back_inserter(toPlay));

	/*
	for (auto it : toPlay)
	{
		type = it->getType();
		if (type == "canc") toDelete.push_back(it);
	}

	for (auto it : toDelete)
	{
		for (vector<trainedBlock*>::iterator b = toPlay.begin(); b != toPlay.end(); b++)
		{
			if ((*b)->getID() == it->getID())
			{
				delete (*b);
				toPlay.erase(b);
				blocks.erase(it);
			}
		}
	}*/

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

	for (auto it : tblocks)
	{
		trainedBlock* current = it.second;
		block_center = cv::Point(current->getXPos(), current->getYPos());

		if (range.contains(block_center))
		{
			addBlock(current);
		}
	}

	cout << "Function: " << getID() << " contains ";
	for (auto it : blocks) cout << it->getID() << " ";
	cout << endl;
}



