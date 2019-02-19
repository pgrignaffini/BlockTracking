#pragma once
#include "Block.h"
#include <unordered_map>
#include <set>

constexpr int N_TRAINING = 30;

struct xDecr
{
	bool operator() (Block* var1, Block* var2) const  { return (var1->getXPos() > var2->getXPos()); }
};


class trainedBlock : public Block
{

private:
	bool trained; //true if trained for N_TRAINING times
	bool defined; //true after training, if defined is true then trained must be true also
	bool reference; //used by function blocks, needed to set the correct amount of cycles that a function block whose referencing to another needs to execute
	int* cycles; //used only for Function

public:
	int count;
	
	unordered_map<string, int> type_list;
	set<trainedBlock*, xDecr>* blocks;

	trainedBlock(); 
	~trainedBlock();
	trainedBlock(const trainedBlock & _block);

	void setBlocks(set<trainedBlock*, xDecr>* _blocks);
	void setDefined(bool def);
	void setTrained(bool train);
	void setReference(bool ref);

	bool isTrained() const;
	bool isDefined() const;
	bool isAReference() const;

	/*******FUNCTION UTILITIES**************/
	void reset_cycles();
	void setCycles(int* _cycles);
	int* getCycles() const; 
	void incrementCycles();
	/***************************************/


	void addType(string t);
	string max(unordered_map<string, int> list);
	
	virtual void play();
	virtual cv::Rect2f findRange(cv::Point2f br);
	virtual void findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& blocks);
	virtual void printRange(cv::Mat cameraFeed);
	virtual void countCycles(cv::Mat threshold); //implemented only in Function.cpp
	virtual set<trainedBlock*, xDecr>* getBlocks() const;
};

