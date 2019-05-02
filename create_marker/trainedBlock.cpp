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
	blocks = set<trainedBlock*, xDecr>();
	cycles = new int(0);
	playing = new bool(false);
	interrupted = new bool(false);
}

trainedBlock::~trainedBlock()
{
	
}

trainedBlock::trainedBlock(const trainedBlock & _block) : Block(_block)
{
	setTrained(_block.isTrained());
	setDefined(_block.isDefined());
	setReference(_block.isAReference());
	setLastLine(_block.isLastLine());
	setLooping(_block.looping);
	setBlocks(_block.getBlocks());
	setCycles(_block.getCycles());
	setPlaying(_block.playing);
	setInterruption(_block.interrupted);
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

void trainedBlock::setLooping(bool * loop)
{
	trainedBlock::looping = loop;
}

void trainedBlock::setPlaying(bool * isPlaying)
{
	trainedBlock::playing = isPlaying;
}

void trainedBlock::setInterruption(bool * interrupt)
{
	trainedBlock::interrupted = interrupt;
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

void trainedBlock::setUpConf(ConfigurationManager * cm)
{
}
