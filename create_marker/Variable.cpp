#include "stdafx.h"
#include "Variable.h"


Variable::Variable()
{
}


Variable::~Variable()
{
}

Variable::Variable(const trainedBlock& n) : trainedBlock(n)
{
	range = cv::Rect2f();
	defined = false;
}


set<trainedBlock*, xDecr>* Variable::getBlocks() const
{
	return Variable::blocks;
}

cv::Rect2f Variable::getRange()
{
	return Variable::range;
}

bool Variable::isDefined()
{
	return Variable::defined;
}

void Variable::setNotes(set<trainedBlock*, xDecr>* _notes)
{
	Variable::blocks = _notes;
}

void Variable::setRange(cv::Rect2f _range)
{
	Variable::range = _range;
}

void Variable::addNote(trainedBlock * _note)
{
	this->blocks->insert(_note);
}

cv::Rect2f Variable::findRange(cv::Point2f br)
{
	int x = getXPos();
	int y = getYPos() + BLOCK_HEIGHT / 2;

	cv::Point2f top_left = cv::Point2f(x, y);
	cv::Point2f bottom_right = cv::Point2f(br.x, y - BLOCK_HEIGHT);

	cv::Rect2f range = cv::Rect2f(bottom_right, top_left);

	setRange(range);
	return range;
}

void Variable::printRange(cv::Mat cameraFeed)
{
	cv::rectangle(cameraFeed, getRange(), cv::Scalar(0, 255, 0));
}

void Variable::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks)
{
	cv::Rect2f range = findRange(br);
	setRange(range);

	cv::Point2f block_center;

	for (auto it : tblocks)
	{
		trainedBlock* current = it.second;

		if (current->getType() == "note") //variables can just contain notes
		{
			block_center = cv::Point2f(current->getXPos(), current->getYPos());
			
			if (range.contains(block_center))
			{
				addNote(current);
				cout << "Added note: " << current->getID() << endl;
			}
		}
		
	}

	cout << "Variable: " << getID() << " contains ";
	for (auto it : *blocks) cout << it->getID() << " ";
	cout << endl;

}

void Variable::play()
{
	cout << "Variable " << getID() << " plays" << endl;
	string type;
	vector<trainedBlock*> toDelete;

	for (auto it : *blocks)
	{
		type = it->getType();
		
		if (type != "canc") it->play();
		
		else toDelete.push_back(it);
	}

	for (auto it : toDelete)
	{
		delete it;
		blocks->erase(it);
	}

}
 


