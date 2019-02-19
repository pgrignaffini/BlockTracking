#include "stdafx.h"
#include "Function.h"

Function::Function()
{

}

Function::~Function()
{

}

Function::Function(const trainedBlock &b) : trainedBlock(b)
{
	range = cv::Rect2f();
	looping = false;
}

set<trainedBlock*, xDecr>* Function::getBlocks() const
{
	return Function::blocks;
}

cv::Rect2f Function::getRange()
{
	return Function::range;
}

void Function::setBlocks(set<trainedBlock*, xDecr>* _blocks)
{
	Function::blocks = _blocks;
}

void Function::setRange(cv::Rect2f _range)
{
	Function::range = _range;
}

void Function::setLoop(bool isLoop)
{
	Function::looping = isLoop;
}

void Function::addBlock(trainedBlock* block)
{
	Function::blocks->insert(block);
}

bool Function::isLoop()
{
	return Function::looping;
}

void Function::play()
{
	cout << "Function " << getID() << " plays" << endl;
	cout << "Function " << getID() << " contains " << *getCycles() << " cycles" << endl;
	string type;
	vector<trainedBlock*> toDelete;

	int ncycles = *getCycles();

	for (auto it : *blocks)
	{
		type = it->getType();

		if (type == "canc") toDelete.push_back(it);
	}

	for (auto it : toDelete)
	{
		delete it;
		blocks->erase(it);
	}

	
	while (looping)
	{
		for (auto it : *blocks)
		{
			it->play();
		}
	}
	
	
	for (int j = 0; j < ncycles; j++)
	{
		for (auto it : *blocks)
		{
			it->play();
		}
	}
	
}

cv::Rect2f Function::findRange(cv::Point2f br)
{
	int x = getXPos();
	int y = getYPos() + BLOCK_HEIGHT / 2;

	cv::Point2f top_left = cv::Point2f(x, y);
	cv::Point2f bottom_right = cv::Point2f(br.x, y - BLOCK_HEIGHT);

	cv::Rect2f range = cv::Rect2f(bottom_right, top_left);

	setRange(range);
	return range;
}

void Function::printRange(cv::Mat cameraFeed)
{
	cv::rectangle(cameraFeed, getRange(), cv::Scalar(0, 255, 0));
}

void Function::findNotes(cv::Point2f br, unordered_map<int, trainedBlock*>& tblocks)
{
	if (isAReference()) return;

	cv::Rect2f range = findRange(br);
	cv::Point2f block_center;

	for (auto it : tblocks)
	{
		trainedBlock* current = it.second;
		block_center = cv::Point2f(current->getXPos(), current->getYPos());

		if (range.contains(block_center))
		{
			addBlock(current);
		}
	}

	cout << "Function: " << getID() << " contains ";
	for (auto it : *blocks) cout << it->getID() << " ";
	cout << endl;
}

void Function::countCycles(cv::Mat threshold_plus)
{
	if (isAReference()) return; //if is a reference block it must not recount its cycles since they are already defined in the block that its referencing

	setLoop(false);

	cv::Mat temp, canny_out;
	cv::Rect range = getRange();

	reset_cycles();
	//cout << "area" << range.area() << endl;

	if (range.area() > 0)
	{
		threshold_plus.copyTo(temp); 
		//these two vectors needed for output of findContours
		vector<vector<cv::Point>> contours;
		vector<cv::Vec4i> hierarchy;

		Canny(temp, canny_out, 0, 50, 5);
		//imshow("Canny", canny_out);

		//find contours of filtered image using openCV findContours function
		findContours(canny_out, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		//contoursConvexHull(contours);

		cv::RNG rng(12345);
		
		/// Draw contours
		cv::Mat drawing = cv::Mat::zeros(canny_out.size(), CV_8UC3);
		for (size_t i = 0; i < contours.size(); ++i)
		{
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		}

		/// Show in a window
		cv::namedWindow("Contours", cv::WINDOW_AUTOSIZE);
		imshow("Contours", drawing);


		int index = 0;
		int xPos, yPos;

		if (hierarchy.size() > 0)
		{
			for (; index >= 0; index = hierarchy[index][0])
			{
				cv::Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				cout << "Area: " << area << endl;

				if (area > 10 * 10)
				{

					xPos = moment.m10 / area;
					yPos = moment.m01 / area;

					cv::Point blockPosition = cv::Point(xPos, yPos);

					//circle(cameraFeed, plusPosition, 10, cv::Scalar(0, 255, 0));

					if (range.contains(blockPosition)) //rect for plus blocks
					{
						if (area < 30 * 30) //black token block has an area of approximately 620px
						{
							incrementCycles();
						}

						else //infinite loop block has an area of approximately 2000px
						{
							cout << "Loop found" << endl;
							setLoop(true);
						}
					}
				}

			}
		}
	}
	
}


