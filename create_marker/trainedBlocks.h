#pragma once
#include "BlockDetector.h"
#include "Function.h"

class trainedBlocks
{
public:

	unordered_map<int, trainedBlock*> tBlocks;

	trainedBlocks();
	~trainedBlocks();

	unordered_map<int, trainedBlock*> getTrainedBlocks();

	//returns the current block with the identifier, if it already exists returns a pointer to that block if it doesn't already
	//exist it inserts it in trainedBlocks, this function also updates the current block position and trains the block
	trainedBlock* update(BlockDetector* detector, ConfigurationManager* config, int XPos, int YPos, vector<vector<cv::Point>> contours, int index);

	//statically set block information based on its id
	trainedBlock* setCorrespondingBlock(int id, unordered_map<int, trainedBlock*>& tBlocks, trainedBlock* tb, ConfigurationManager* config);

};

