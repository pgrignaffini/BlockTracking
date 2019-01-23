#include "stdafx.h"
#include "Bracket.h"


Bracket::Bracket()
{ 
}


Bracket::~Bracket()
{
}


Bracket::Bracket(const trainedBlock &b) : trainedBlock(b)
{
}

void Bracket::setPos(string _pos)
{
	Bracket::pos = _pos;
}

string Bracket::getPos()
{
	return Bracket::pos;
}

void Bracket::setOtherBracket(Bracket * other)
{
	Bracket::otherBracket = other;
}

Bracket * Bracket::getOtherBracket()
{
	return Bracket::otherBracket;
}

void Bracket::setCycles(int number)
{
	Bracket::cycleNumber = number;
}

int Bracket::getCycles()
{
	return Bracket::cycleNumber;
}

void Bracket::setRange(cv::Rect _range)
{
	Bracket::range = _range;
}

cv::Rect Bracket::getRange()
{
	return Bracket::range;
}

void Bracket::findMatch(vector<Bracket*> tracked)
{
	int xPos = getXPos();
	int yPos = getYPos();

	for (int i = 0; i < tracked.size(); i++)
	{

		if (tracked.at(i)->getYPos() <= yPos + 10 && tracked.at(i)->getYPos() >= yPos - 10) //brackets could not be perfectly aligned
			//support just brackets placed on the same line
		{
			if (xPos > tracked.at(i)->getXPos())
			{
				setPos("end");
				tracked.at(i)->setPos("start");
			}

			else
			{
				setPos("start");
				tracked.at(i)->setPos("end");
			}

			setOtherBracket(tracked.at(i));
			return; //we found the matching bracket
		}
	}

	return;
}


void Bracket::calculateRange(cv::Mat cameraFeed)
{

	int xPos = getXPos();
	int yPos = getYPos();
	cv::Point p0 = (xPos, yPos);

	Bracket* match = getOtherBracket();

	if (match != NULL)
	{

		int distance = fabs(match->getXPos() - xPos); //distance between the two brackets

		cv::Rect range(xPos, yPos - 50, distance, 100); //50 pixel is the distance between the bracket's center and its top/bottom
		cv::rectangle(cameraFeed, range, cv::Scalar(0, 0, 255));
		setRange(range);
		otherBracket->setRange(range);

	}

}

void Bracket::findPlusBlocks(cv::Mat threshold_plus, cv::Mat cameraFeed)
{
	cv::Mat temp, canny_out;
	Block plus;

	cv::Rect range = getRange();
	//cout << "area" << range.area() << endl;

	int numObjects = 0;

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

		int index = 0;
		int xPos, yPos;

		for (; index >= 0; index = hierarchy[index][0])
		{
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;

			if (area > 10 * 10)
			{
				xPos = moment.m10 / area;
				yPos = moment.m01 / area;

				cv::Point plusPosition = cv::Point(xPos, yPos);

				circle(cameraFeed, plusPosition, 10, cv::Scalar(0,255, 0));

				if (range.contains(plusPosition)) //rect for plus blocks
				{
					numObjects++;
				}
			}

		}
	}

	setCycles(numObjects);
	Bracket* match = getOtherBracket();
	match->setCycles(numObjects);
	return;

}


void Bracket::drawBracket(cv::Mat cameraFeed)
{
	cv::putText(cameraFeed, getPos(), cv::Point(getXPos(), getYPos() + 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1.5);
	cv::putText(cameraFeed, std::to_string(getCycles()), cv::Point(getXPos(), getYPos() + 40), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1.5);
}