#pragma once
#include "stdafx.h"
#include "BlockDetector.h"

using namespace std;
using namespace cv;

BlockDetector::BlockDetector()
{
	feed = new CameraCalibration();
}

BlockDetector::~BlockDetector()
{
}

//Getters


std::vector<int> BlockDetector::getIDs()
{
	return BlockDetector::IDs;
}

vector<vector<Point2f>> BlockDetector::getIdentifiedCorners()
{
	return BlockDetector::markerCorners;
}

cv::Rect BlockDetector::getBLine()
{
	return BlockDetector::bline;
}


//Setters

void BlockDetector::setIDs(vector<int> ids)
{
	BlockDetector::IDs = ids;
}


void BlockDetector::setCorners(std::vector<std::vector<cv::Point2f>> corners)
{
	BlockDetector::markerCorners = corners;
}

void BlockDetector::setBLine(cv::Rect _bline)
{
	BlockDetector::bline = _bline;
}

//Utilities

double BlockDetector::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

//Core functionalities

void BlockDetector::findCorners(Mat& cameraFeed)
{
	vector<int> markerIdentifiers;
	vector<vector<Point2f>> corners, rejectedCandidates;
	vector<Vec3d> rotationVectors, translationVectors;
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;

	aruco::DetectorParameters parameters;
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	//namedWindow("Find markers", WINDOW_AUTOSIZE);

	aruco::detectMarkers(cameraFeed, markerDictionary, corners, markerIdentifiers);
	//for (int i = 0; i < markerIdentifiers.size(); i++) cout << markerIdentifiers.at(i) << " ";
	aruco::estimatePoseSingleMarkers(corners, feed->getArucoSquareDimension(), cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
	aruco::drawDetectedMarkers(cameraFeed, corners, markerIdentifiers); //show IDs

	setIDs(markerIdentifiers);
	setCorners(corners);
}

string BlockDetector::detectType(std::vector<std::vector<cv::Point>>& contours, int i)
{
	string block_type;
	vector<cv::Point> approx;

	approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);
	//cout << "Vertex" << nvertex << endl;
	//std::cout << "Block size: " << blocks.size();

	int approx_size = approx.size();

	if (approx_size >= 4)
	{
		// Number of vertices of polygonal curve
		int vtc = approx.size();

		// Get the cosines of all corners
		vector<double> cos;
		for (int j = 2; j < vtc + 1; j++)
			cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

		// Sort ascending the cosine values
		sort(cos.begin(), cos.end());

		// Get the lowest and the highest cosine
		double mincos = cos.front();
		double maxcos = cos.back();

		// Use the degrees obtained above and the number of vertices
		// to determine the shape of the contour
		if (approx_size == 4 && mincos >= -0.1 && maxcos <= 0.3)
		{
			cv::Rect r = boundingRect(contours[i]);
			double ratio = abs(1 - (double)r.width / r.height);

			block_type = "variable";

			/* Code for Break block detection

			if (ratio <= 0.15)
			{
				block_type = "variable";
			}

			else
			{
				block_type = "break";
			}*/
		}

		else if (approx_size == 5 || approx_size == 6 || approx_size == 7) //noise makes the pentagon cut on the top, so the vertices are 6
		{
			block_type = "function";
		}

		else if (approx_size >= 8 && maxcos < 0.3)
		{

			block_type = "note";

			/* Code for Bracket block detection

			double area = contourArea(contours[i]);
			cv::Rect r = boundingRect(contours[i]);
			int radius = r.width / 2;

			
			if (abs(1 - ((double)r.width / r.height)) <= 0.3 &&
				abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.3)
			{
				block_type = "note";
			}

			else block_type = "bracket";
			*/
		}

	}

	return block_type;
}

trainedBlock * BlockDetector::update(unordered_map<int, trainedBlock*>& tBlocks, ConfigurationManager* config, int XPos, int YPos, vector<vector<cv::Point>> contours, int index)
{
	trainedBlock* tb = new trainedBlock();
	vector<std::vector<cv::Point2f>> corners = getIdentifiedCorners();
	vector<int> ids = getIDs();

	int id = findIdentifier(corners, ids, XPos, YPos);
	string type;
	cv::Point center;

	//look if the block has been already inserted
	unordered_map<int, trainedBlock*>::iterator found = tBlocks.find(id);

	if (found != tBlocks.end()) //block already identified
	{
		found->second->setXPos(XPos);
		found->second->setYPos(YPos);

		center = cv::Point(XPos, YPos);

		//set property for blocks positioned on the last line
		if (bline.contains(center)) found->second->setLastLine(true);

		if (!found->second->isTrained()) //training
		{
			type = detectType(contours, index);
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
			/*
			else if (type == "variable")
			{
				Variable* var = new Variable(*found->second);
				delete(found->second);
				found->second = var;
			}*/

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

		tb = setCorrespondingBlock(id, tBlocks, tb, config);

		tb->setXPos(XPos);
		tb->setYPos(YPos);
		tb->setID(id);

		type = detectType(contours, index);
		tb->addType(type);

		tBlocks.insert({ id, tb }); //insert new pair value

		return tb;
	}

	else return nullptr;

}

trainedBlock * BlockDetector::setCorrespondingBlock(int id, unordered_map<int, trainedBlock*>& tBlocks, trainedBlock* tb, ConfigurationManager* config)
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
		found = tBlocks.find(depsOf[i]);

		if (found != tBlocks.end() && !found->second->isAReference())
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
}


trainedBlock* BlockDetector::findObject(cv::Mat threshold, std::unordered_map<int, trainedBlock*>& tBlocks, ConfigurationManager* config)
{
	trainedBlock* aBlock = new trainedBlock();
	//use moments method to find our filtered object
	cv::Mat temp, canny_out;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<vector<cv::Point>> contours;

	threshold.copyTo(temp);
	//Canny(temp, canny_out, thresh, thresh * 2, 3);
	Canny(temp, canny_out, 0, 50, 5);

	//find contours of filtered image using openCV findContours function
	findContours(canny_out, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	double refArea = 0;


	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size();
		//cout << "Objects detected: " << numObjects << endl;
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS)
		{
			int index = 0;
			for (; index >= 0; index = hierarchy[index][0])
			{

				cv::Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.

				if (area > MIN_OBJECT_AREA)
				{
					try
					{

						int X = moment.m10 / area;
						int Y = moment.m01 / area;

						//returns the current block with the identifier
						//if it already exists returns a pointer to that block
						//if it doesn't already exist it inserts it in trainedBlocks
						//the function also updates the current block position and trains the block
						aBlock = update(tBlocks, config, X, Y, contours, index);

						if (aBlock == nullptr) return aBlock;

						
					}
					catch (exception& e)
					{
						std::cout << "Something went wrong: " << e.what() << '\n';
					}
				}

			}
		}
	}

	return aBlock;
}

//this function identifies the right marker ID to assign to the block in consideration, the point <xm,ym> represents its center
int BlockDetector::findIdentifier(InputArrayOfArrays _corners, InputArray _ids, int xm, int ym)
{
	int nMarkers = (int)_corners.total();
	int idFound = -1;

	Rect blockArea = Rect(xm - BLOCK_WIDTH / 2, ym - BLOCK_HEIGHT / 2, BLOCK_WIDTH, BLOCK_HEIGHT);

	for (int i = 0; i < nMarkers; i++)
	{
		
		Mat currentMarker = _corners.getMat(i);
		//CV_Assert(currentMarker.total() == 4 && currentMarker.type() == CV_32FC2);

		if (_ids.total() != 0)
		{		
			Point2f cent(0, 0);
			for (int p = 0; p < 4; p++)
				cent += currentMarker.ptr<Point2f>(0)[p];
			cent = cent / 4.;

			if (blockArea.contains(cent))
			{
				idFound = _ids.getMat().ptr<int>(0)[i];
				return idFound;
			}
		}
	}

	return idFound;
}

void BlockDetector::check_for_changes(unordered_map<int, trainedBlock*>& tBlocks)
{
	vector<int> alive = getIDs();
	vector<int> saved;
	int id;
	trainedBlock* current;
	vector<set<trainedBlock*, xDecr>::iterator> toErase;

	//cout << "Checking for changes on the board..." << endl;
	
	//erase blocks no longer on board
	bool found;

	for (unordered_map<int, trainedBlock*>::iterator it = tBlocks.begin(); it != tBlocks.end(); ++it)
	{
		saved.push_back(it->first);
	}

	for (int i = 0; i < saved.size(); ++i)
	{

		found = false;
		for (int j = 0; j < alive.size(); ++j)
		{
			if (saved.at(i) == alive.at(j))
			{
				found = true;
				break;
			}

		}

		if (!found) 
		{
			auto it = tBlocks.find(saved.at(i));
			id = it->first;
			it->second->setType("canc"); //needed for deleting inside the blocks, see play() function
			cout << id << " not found, erasing..." << endl;
			tBlocks.erase(id);
		}
	}

	return;

}

void BlockDetector::trackFilteredObject(cv::Mat threshold, cv::Mat &cameraFeed, ConfigurationManager* config, unordered_map<int, trainedBlock*>& tBlocks)
{
	//cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	//cv::Mat distanceCoefficients;
	//these two vectors needed for output of findContours
	bool objectFound;

	findCorners(cameraFeed);
	trainedBlock* blockFound = findObject(threshold, tBlocks, config);

	if (blockFound != nullptr) objectFound = true;
	else objectFound = false;

	/*
	/// Draw contours
	Mat drawing = Mat::zeros(canny_out.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); ++i)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}*/

	/// Show in a window
	//namedWindow("Contours", WINDOW_AUTOSIZE);
	//imshow("Contours", drawing);

	//erase blocks no longer on board
	check_for_changes(tBlocks);


	cout << "Identified Blocks: ";
	for (auto& it : tBlocks) cout << it.first << " ";
	cout << endl;

	if (!objectFound) putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", cv::Point(0, 50), 1, 2, cv::Scalar(0, 0, 255), 2);

	return;
}