#pragma once
#include "stdafx.h"
#include "trainedBlock.h"

trainedBlock::trainedBlock() : Block()
{
	trained = false;
	defined = false;
	reference = true;
	lastLine = false;
	looping = new bool(false);

	count = 0;
	blocks = set<trainedBlock*, xDecr>();
	cycles = new int(0);
}

trainedBlock::~trainedBlock()
{

}

trainedBlock::trainedBlock(const trainedBlock & _block) : trainedBlock()
{

	setType(_block.getType());
	setID(_block.getID());
	setXPos(_block.getXPos());
	setYPos(_block.getYPos());
	setHSVmin(_block.getHSVmin());
	setHSVmax(_block.getHSVmax());
	setBlocks(_block.getBlocks());
	setCycles(_block.getCycles());

	setTrained(_block.isTrained());
	setDefined(_block.isDefined());
	setReference(_block.isAReference());

	count = 0;
}

void trainedBlock::setBlocks(set<trainedBlock*, xDecr> _blocks)
{
	trainedBlock::blocks = _blocks;
}

void trainedBlock::setDefined(bool def)
{
	trainedBlock::defined = def;
}

void trainedBlock::setTrained(bool train)
{
	trainedBlock::trained = train;
}

void trainedBlock::setReference(bool ref)
{
	trainedBlock::reference = ref;
}

void trainedBlock::setLastLine(bool ll)
{
	trainedBlock::lastLine = ll;
}

bool trainedBlock::isTrained() const
{
	return trainedBlock::trained;
}

bool trainedBlock::isDefined() const
{
	return trainedBlock::defined;
}


bool trainedBlock::isAReference() const
{
	return trainedBlock::reference;
}

bool trainedBlock::isLastLine() const
{
	return trainedBlock::lastLine;
}

int* trainedBlock::getCycles() const
{
	return trainedBlock::cycles;
}

void trainedBlock::setCycles(int* _cycles)
{
	trainedBlock::cycles = _cycles;
}

void trainedBlock::reset_cycles()
{
	*cycles = 0;
}

void trainedBlock::incrementCycles()
{
	(*cycles)++;
}


void trainedBlock::addType(string t)
{
	if (t.empty()) return;

	//setType(t); //overwrite 'X'
	
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


int trainedBlock::play(int channel)
{
	return -1;
}

void trainedBlock::playParallel()
{
}

cv::Rect2f trainedBlock::findRange(cv::Point2f br)
{
	return cv::Rect2f();
}

void trainedBlock::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*> blocks)
{
}

void trainedBlock::printRange(cv::Mat cameraFeed)
{
}

void trainedBlock::countCycles(cv::Mat threshold)
{
}

set<trainedBlock*, xDecr> trainedBlock::getBlocks() const
{
	return trainedBlock::blocks;
}
