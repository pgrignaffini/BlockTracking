#pragma once
#include "trainedBlock.h"
#include "Note.h"
#include "Variable.h"

class Function : public trainedBlock
{

private:
	cv::Rect2f range;
	int cycles;
	bool defined;

public:
	Function();
	~Function();

	Function(const trainedBlock &b);

	int getCycles();
	cv::Rect2f getRange();
	bool isDefined();

	void setBlocks(set<trainedBlock*, xDecr>* _blocks);
	void setRange(cv::Rect2f _range);
	
	void incrementCycles(int n);
	void addBlock(trainedBlock* block);
	void reset_cycles();
	 
	void play() override;
	cv::Rect2f findRange(cv::Point2f br) override;
	void printRange(cv::Mat cameraFeed) override;
	void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks) override;
	void countCycles(cv::Mat threshold_plus) override;
	set<trainedBlock*, xDecr>* getBlocks() const override;
};

