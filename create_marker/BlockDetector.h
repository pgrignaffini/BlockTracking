#pragma once
#include "stdafx.h"
#include "trainedBlock.h"
#include "ConfigurationManager.h"
#include "Note.h"
#include "Function.h"
#include "CameraCalibration.h"

class BlockDetector
{

private:
	std::vector<int> IDs; //identified aruco ids
	std::vector<std::vector<cv::Point2f>> markerCorners; //identified aruco ids' corners
	cv::Rect bline; //board's bottom line
	CameraCalibration* feed; //detection depends on specified camera calibration

public:
	BlockDetector();
	~BlockDetector();

	std::vector<int> getIDs();
	std::vector<std::vector<cv::Point2f>> getIdentifiedCorners();
	cv::Rect getBLine();

	void setIDs(std::vector<int> ids);
	void setCorners(std::vector<std::vector<cv::Point2f>> corners);
	void setBLine(cv::Rect _bline);

	double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

	void findCorners(cv::Mat& cameraFeed);
	int findIdentifier(cv::InputArrayOfArrays _corners, cv::InputArray _ids, int xm, int ym);
	void check_for_changes(std::unordered_map<int, trainedBlock*>& tBlocks);
	std::string detectType(std::vector<std::vector<cv::Point>>& contours, int i);
	trainedBlock* findObject(cv::Mat threshold, std::unordered_map<int, trainedBlock*>&  tBlocks, ConfigurationManager* config);
	trainedBlock* update(unordered_map<int, trainedBlock*>& tBlocks, ConfigurationManager* config, int XPos, int YPos, std::vector<std::vector<cv::Point>> contours, int index);
	//statically set block information based on its id
	bool checkIfReference(unordered_map<int, trainedBlock*>& tBlocks, trainedBlock* thisBlock, ConfigurationManager* config);
	void trackFilteredObject(cv::Mat threshold, cv::Mat & cameraFeed, ConfigurationManager * config, unordered_map<int, trainedBlock*>& tBlocks);

};

