#pragma once
#include "Note.h"



class Variable : public trainedBlock
{

private:
	cv::Rect2f range;

public:
	
	Variable();
	~Variable();
	Variable(const trainedBlock &n);

	cv::Rect2f getRange();
	bool isDefined();

	void setNotes(set<trainedBlock*, xDecr>* _notes);
	void setRange(cv::Rect2f _range);

	void addNote(trainedBlock* _note);
	
	void play() override;
	cv::Rect2f findRange(cv::Point2f br) override;
	void printRange(cv::Mat cameraFeed) override;
	void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks) override;
	set<trainedBlock*, xDecr>* getBlocks() const override;
};


