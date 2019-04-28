#pragma once

class CameraCalibration
{

private:
	float calibrationSquareDimension;
	float arucoSquareDimension;
	cv::Size chessboardDimensions; //number of intersection points on the chessboard

public:
	CameraCalibration();
	~CameraCalibration();

	float getCalibrationSquareDimension();

	float getArucoSquareDimension();

	cv::Size getChessboardDimensions();

	void setCalibrationSquareDimension(float calib);

	void setArucoSquareDimension(float aruc);

	void setChessboardDimensions(cv::Size size);

	void createArucoMarkers();
	void createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners);
	void getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults = false); // find and visualize chessboard corners
	void camera_calibration(std::vector<cv::Mat>& calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	bool saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients);
	bool loadCameraCalibration(std::string name, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
	void cameraCalibrationProcess(cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients);
};

