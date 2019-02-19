#pragma once
#include "trainedBlock.h"
#include "Note.h"
#include "Variable.h"

class Function : public trainedBlock
{

private:
	cv::Rect2f range;
	bool looping;

public:
	Function();
	~Function();

	Function(const trainedBlock &b);

	
	cv::Rect2f getRange();

	void setBlocks(set<trainedBlock*, xDecr>* _blocks);
	void setRange(cv::Rect2f _range);
	void setLoop(bool isLoop);
	
	void addBlock(trainedBlock* block);

	bool isLoop();
	 
	void play() override;
	cv::Rect2f findRange(cv::Point2f br) override;
	void printRange(cv::Mat cameraFeed) override;
	void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks) override;
	void countCycles(cv::Mat threshold_plus) override;
	set<trainedBlock*, xDecr>* getBlocks() const override;
};

