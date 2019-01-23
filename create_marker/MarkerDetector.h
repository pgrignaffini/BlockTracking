#pragma once

#include "Block.h"

#include <opencv2\calib3d.hpp>
#include <opencv2\highgui.hpp>

class MarkerDetector
{
private:
	float calibrationSquareDimension = 0.026f; //meters = 2.6cm
	float arucoSquareDimension = 0.015f; // 1.5cm
	cv::Size chessboardDimensions = cv::Size(6, 9); //number of intersection points on the chessboard
	std::vector<int> IDs;

public:
	MarkerDetector();
	~MarkerDetector();

	float getCalibrationSquareDimension();
	float getArucoSquareDimension();
	cv::Size getChessboardDimensions();
	std::vector<int> getIDs();

	void setCalibrationSquareDimension(float calib);
	void setArucoSquareDimension(float aruc);
	void setChessboardDimensions(cv::Size size);
	void setIDs(vector<int> ids);

	void createArucoMarkers();
	void createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners);
	void getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults = false); // find and visualize chessboard corners
	void camera_calibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	bool saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients);
	bool loadCameraCalibration(std::string name, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	void cameraCalibrationProcess(cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);

	std::vector<std::vector<cv::Point2f>> findCorners(cv::Mat cameraFeed, MarkerDetector& detective);
	int findId(int x, int y, std::vector<std::vector<cv::Point2f>> identifiedCorners);

};

