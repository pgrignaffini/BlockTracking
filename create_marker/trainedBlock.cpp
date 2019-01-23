#include "stdafx.h"
#include "trainedBlock.h"

trainedBlock::trainedBlock()
{
	Block();
	trained = false;
	defined = false;
	count = 0;
	type_list["variable"] = 0;
	type_list["bracket"] = 0;
	type_list["function"] = 0;
	type_list["note"] = 0;
	type_list["break"] = 0;

	blocks = new set<trainedBlock*, xDecr>();
}

trainedBlock::~trainedBlock()
{

}

trainedBlock::trainedBlock(const trainedBlock & _block)
{
	setType(_block.getType());
	setID(_block.getID());
	setXPos(_block.getXPos());
	setYPos(_block.getYPos());
	setHSVmin(_block.getHSVmin());
	setHSVmax(_block.getHSVmax());
	setBlocks(_block.getBlocks());

	trained = _block.isTrained();
	defined = _block.isDefined();
	type_list = _block.type_list;
	count = 0;
	cycles = 0;
}

void trainedBlock::setBlocks(set<trainedBlock*, xDecr>* _blocks)
{
	trainedBlock::blocks = _blocks;
}

void trainedBlock::setCycles(int _cycles)
{
	cycles = _cycles;
}

bool trainedBlock::isTrained() const
{
	return trainedBlock::trained;
}

bool trainedBlock::isDefined() const
{
	return trainedBlock::defined;
}

int trainedBlock::getCycles() const
{
	return trainedBlock::cycles;
}

void trainedBlock::addType(string t)
{
	if (t.empty()) return;

	setType(t); //overwrite 'X'
	type_list[t]+=1;
	count+=1;

	if (count == N_TRAINING)
	{
		trained = true;
		setType(max(type_list));
	}

	return;

}

string trainedBlock::max(unordered_map<string, int> list)
{
	int max = 0;
	string max_index;

	for (auto it = list.begin(); it != list.end(); ++it)
	{
		if (it->second > max)
		{
			max = it->second;
			max_index = it->first;
		}
	}

	return max_index;
}

void trainedBlock::play()
{
}

cv::Rect2f trainedBlock::findRange(cv::Point2f br)
{
	return cv::Rect2f();
}

void trainedBlock::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& blocks)
{
}

void trainedBlock::printRange(cv::Mat cameraFeed)
{
}

void trainedBlock::countCycles(cv::Mat threshold)
{
}

set<trainedBlock*, xDecr>* trainedBlock::getBlocks() const
{
	return trainedBlock::blocks;
}
