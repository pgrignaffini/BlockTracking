#pragma once
#include "Block.h"
#include <unordered_map>
#include <set>

struct xDecr
{
	bool operator() (Block* var1, Block* var2) const  { return (var1->getXPos() > var2->getXPos()); }
};


#define N_TRAINING 10

class trainedBlock : public Block
{
public:

	bool trained;
	bool defined;
	int count;
	int cycles;
	unordered_map<string, int> type_list;
	set<trainedBlock*, xDecr>* blocks;

	trainedBlock(); 
	~trainedBlock();
	trainedBlock(const trainedBlock & _block);

	void setBlocks(set<trainedBlock*, xDecr>* _blocks);
	void setCycles(int _cycles);

	bool isTrained() const;
	bool isDefined() const;
	int getCycles() const;
	void addType(string t);
	string max(unordered_map<string, int> list);
	
	virtual void play();
	virtual cv::Rect2f findRange(cv::Point2f br);
	virtual void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& blocks);
	virtual void printRange(cv::Mat cameraFeed);
	virtual void countCycles(cv::Mat threshold); //implemented only in Function.cpp
	virtual set<trainedBlock*, xDecr>* getBlocks() const;
};

