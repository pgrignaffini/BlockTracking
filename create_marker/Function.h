#pragma once
#include "trainedBlock.h"
#include <SDL_mixer.h>

class Function : public trainedBlock
{

private:
	cv::Rect2f range;

public:
	Function();
	~Function();

	Function(const trainedBlock &b);
	
	cv::Rect2f getRange();

	void setBlocks(set<trainedBlock*, xDecr>* _blocks);
	void setRange(cv::Rect2f _range);

	void addBlock(trainedBlock* block);
	bool firstInLine();
	 
	int play(int channel)override;
	cv::Rect2f findRange(cv::Point2f br) override;
	void printRange(cv::Mat cameraFeed) override;
	void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks) override;
	set<trainedBlock*, xDecr>* getBlocks() const override;
};

