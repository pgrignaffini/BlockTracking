#pragma once

#include "stdafx.h"
#include "trainedBlock.h"

class BlockDetector
{

private:

	float calibrationSquareDimension; 
	float arucoSquareDimension; 
	cv::Size chessboardDimensions; //number of intersection points on the chessboard
	std::vector<int> IDs;
	std::vector<std::vector<cv::Point2f>> markerCorners;


public:
	BlockDetector();
	~BlockDetector();

	float getCalibrationSquareDimension();
	float getArucoSquareDimension();
	cv::Size getChessboardDimensions();
	std::vector<int> getIDs();
	std::vector<std::vector<cv::Point2f>> getIdentifiedCorners();

	void setCalibrationSquareDimension(float calib);
	void setArucoSquareDimension(float aruc);
	void setChessboardDimensions(cv::Size size);
	void setIDs(vector<int> ids);
	void setCorners(std::vector<std::vector<cv::Point2f>> corners);

	void createArucoMarkers();
	void createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners);
	void getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults = false); // find and visualize chessboard corners
	void camera_calibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	bool saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients);
	bool loadCameraCalibration(std::string name, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	void cameraCalibrationProcess(cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

	void findCorners(cv::Mat& cameraFeed);
	int findIdentifier(cv::InputArrayOfArrays _corners, cv::InputArray _ids, int xm, int ym);
	void check_for_changes(unordered_map<int, trainedBlock*>& tBlocks);
	string detectType(std::vector<std::vector<cv::Point>>& contours, int i);

};

