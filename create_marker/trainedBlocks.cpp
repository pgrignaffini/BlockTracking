#pragma once
#include "stdafx.h"
#include "trainedBlocks.h"

trainedBlocks::trainedBlocks()
{
}


trainedBlocks::~trainedBlocks()
{
}

/*
trainedBlock * trainedBlocks::update(BlockDetector * detector, ConfigurationManager* config, int XPos, int YPos, vector<vector<cv::Point>> contours, int index, cv::Rect bline)
{
	trainedBlock* tb = new trainedBlock();
	vector<std::vector<cv::Point2f>> corners = detector->getIdentifiedCorners();
	vector<int> ids = detector->getIDs();

	int id = detector->findIdentifier(corners, ids, XPos, YPos);
	string type;
	cv::Point center;

	//look if the block has been already inserted
	unordered_map<int, trainedBlock*>::iterator found = this->tBlocks.find(id);

	if (found != this->tBlocks.end()) //block already identified
	{
		found->second->setXPos(XPos);
		found->second->setYPos(YPos);

		center = cv::Point(XPos, YPos);

		//set property for blocks positioned on the last line
		if (bline.contains(center)) found->second->setLastLine(true); 

		if (!found->second->isTrained()) //training
		{
			type = detector->detectType(contours, index);
			found->second->addType(type);
		}

		else if (found->second->isTrained() && !found->second->isDefined()) //cast to correct type after training
		{
			type = found->second->getType();
			if (type == "note")
			{
				Note* note = new Note(*found->second);
				note->setUpConf(config); //set configuration file
				delete(found->second); //delete pointed element
				found->second = note;
			}

			else if (type == "variable")
			{
				Variable* var = new Variable(*found->second);
				delete(found->second);
				found->second = var;
			}

			else if (type == "function")
			{
				Function* func = new Function(*found->second);
				delete(found->second);
				found->second = func;
			}

			found->second->setDefined(true);
		}

		return found->second;
	}

	else if (id != -1) // block not yet registered, it has an ID (aruco marker) but no type
	{
		cout << "New block found: " << id << endl;

		tb = setCorrespondingBlock(id, this->tBlocks, tb, config);

		tb->setXPos(XPos);
		tb->setYPos(YPos);
		tb->setID(id);

		type = detector->detectType(contours, index);
		tb->addType(type);

		this->tBlocks.insert({ id, tb }); //insert new pair value

		return tb;
	}

	else return nullptr;

}

trainedBlock * trainedBlocks::setCorrespondingBlock(int id, unordered_map<int, trainedBlock*>& tBlocks, trainedBlock* tb, ConfigurationManager* config)
{
	int to_find;
	std::vector<int> depsOf;
	std::vector<int>::iterator thisBlock;

	depsOf = config->getDepsOf(id);
	
	if (depsOf.size() > 0)
	{
		thisBlock = std::find(depsOf.begin(), depsOf.end(), id);
		depsOf.erase(thisBlock); //remove element to avoid circular dependencies
		//depsOf contains the ids linked to the block in consideration
	}

	unordered_map<int, trainedBlock*>::iterator found;
	for (int i = 0; i < depsOf.size(); i++)
	{
		found = this->tBlocks.find(depsOf[i]);
		 
		if (found != this->tBlocks.end() && !found->second->isAReference())
		{
			tb->setBlocks(found->second->getBlocks()); //assign the pointer to the vector
			tb->setType(found->second->getType());
			tb->setTrained(found->second->isTrained());
			tb->setCycles(found->second->getCycles()); //assign an integer pointer
			tb->setReference(true); //the new block found is a reference to another one, dependencies are specified in "conf/deps.txt"
			break;
		}

	}

	return tb;
}*/

unordered_map<int, trainedBlock*> trainedBlocks::getTrainedBlocks()
{
	return trainedBlocks::tBlocks;
}


  