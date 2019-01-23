#pragma once
#include "BlockDetector.h"
#include "Note.h"
#include "Variable.h"
#include "Function.h"

class trainedBlocks
{
public:

	unordered_map<int, trainedBlock*> tBlocks;

	trainedBlocks();
	~trainedBlocks();

	//returns the current block with the identifier
	//if it already exists returns a pointer to that block
	//if it doesn't already exist it inserts it in trainedBlocks
	//the function also updates the current block position and trains the block
	trainedBlock* update(BlockDetector* detector, int XPos, int YPos, vector<vector<cv::Point>> contours, int index);
	trainedBlock* setCorrespondingBlock(int id, unordered_map<int, trainedBlock*>& tBlocks, trainedBlock* tb);
	unordered_map<int, trainedBlock*> getTrainedBlocks();
};

