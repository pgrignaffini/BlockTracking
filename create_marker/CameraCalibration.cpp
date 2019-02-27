#include "stdafx.h"
#include "CameraCalibration.h"


CameraCalibration::CameraCalibration()
{
	calibrationSquareDimension = 0.026f; //meters = 2.6cm
	arucoSquareDimension = 0.015f; // 1.5cm
	chessboardDimensions = cv::Size(6, 9); //number of intersection points on the chessboard
}


CameraCalibration::~CameraCalibration()
{
}


float CameraCalibration::getCalibrationSquareDimension()
{
	return CameraCalibration::calibrationSquareDimension;
}

float CameraCalibration::getArucoSquareDimension()
{
	return CameraCalibration::arucoSquareDimension;
}

cv::Size CameraCalibration::getChessboardDimensions()
{
	return CameraCalibration::chessboardDimensions;
}

void CameraCalibration::setCalibrationSquareDimension(float calib)
{
	CameraCalibration::calibrationSquareDimension = calib;
}

void CameraCalibration::setArucoSquareDimension(float aruc)
{
	CameraCalibration::arucoSquareDimension = aruc;
}

void CameraCalibration::setChessboardDimensions(cv::Size size)
{
	CameraCalibration::chessboardDimensions = size;
}

void CameraCalibration::createArucoMarkers()
{
	cv::Mat outputMarker;
	cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	//create markers and print them in a file
	// 1 pixel = 0.26 mm
	//square block = 2cm => 75 pixel
	for (int i = 0; i < 50; i++)
	{
		cv::aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);
		std::ostringstream convert;
		std::string imageName = "4x4Marker_";
		convert << imageName << i << ".jpg";
		imwrite(convert.str(), outputMarker);
	}
}

void CameraCalibration::createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners)
{
	for (int i = 0; i < boardSize.height; i++)
	{
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(cv::Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
		}
	}
}

void CameraCalibration::getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults)
{
	for (std::vector<cv::Mat>::iterator iter = images.begin(); iter != images.end(); iter++)
	{
		std::vector<cv::Point2f> pointBuf; //buffer to store all the corners
		bool found = cv::findChessboardCorners(*iter, chessboardDimensions, pointBuf, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

		if (found)	allFoundCorners.push_back(pointBuf);

		if (showResults)
		{
			drawChessboardCorners(*iter, cv::Size(6, 9), pointBuf, found);
			imshow("Looking for corners", *iter);
			cv::waitKey(0);
		}
	}
}

void CameraCalibration::camera_calibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	std::vector<std::vector<cv::Point2f>> checkerboardImageSpacePoints;
	getChessboardCorners(calibrationImages, checkerboardImageSpacePoints, false);

	std::vector<std::vector<cv::Point3f>> worldSpaceCornerPoints(1);

	createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);

	worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

	std::vector<cv::Mat> rVectors, tVectors;
	distanceCoefficients = cv::Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
}

bool CameraCalibration::saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients)
{
	std::ofstream outStream(name);
	if (outStream)
	{
		uint16_t rows = cameraMatrix.rows;
		uint16_t columns = cameraMatrix.cols;

		outStream << rows << std::endl;
		outStream << columns << std::endl;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << std::endl;
			}
		}

		rows = distanceCoefficients.rows;
		columns = distanceCoefficients.cols;

		outStream << rows << std::endl;
		outStream << columns << std::endl;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double value = distanceCoefficients.at<double>(r, c);
				outStream << value << std::endl;
			}
		}

		outStream.close();
		return true;
	}

	return false;
}

bool CameraCalibration::loadCameraCalibration(std::string name, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	std::ifstream instream(name);
	if (instream)
	{
		uint16_t rows;
		uint16_t columns;

		instream >> rows;
		instream >> columns;

		cameraMatrix = cv::Mat(cv::Size(columns, rows), CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0;
				instream >> read;
				cameraMatrix.at<double>(r, c) = read;
				//cout << cameraMatrix.at<double>(r, c) << "\n";
			}
		}

		//Distance Coefficients
		instream >> rows;
		instream >> columns;

		distanceCoefficients = cv::Mat::zeros(rows, columns, CV_64F);

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double read = 0.0;
				instream >> read;
				distanceCoefficients.at<double>(r, c) = read;
				//cout << distanceCoefficients.at<double>(r, c) << "\n";
			}
		}

		instream.close();
		return true;

	}

	return false;
}

void CameraCalibration::cameraCalibrationProcess(cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	cv::Mat frame;
	cv::Mat drawToFrame;
	std::vector<cv::Mat> savedImages;
	std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

	cv::VideoCapture vid(0);

	if (!vid.isOpened()) return;

	int framesPerSecond = 40;
	cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

	while (true)
	{
		if (!vid.read(frame)) break;

		std::vector<cv::Vec2f> foundPoints;
		bool found = false;

		//found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
		found = findChessboardCorners(frame, chessboardDimensions, foundPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		frame.copyTo(drawToFrame);
		drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);

		if (found)
		{
			imshow("Webcam", drawToFrame);
		}

		else
		{
			imshow("Webcam", frame);
		}

		char character = cv::waitKey(1000 / framesPerSecond); //waitKey returns the pressed key

		switch (character)
		{
		case ' ':
			//saving image
			if (found)
			{
				cv::Mat temp;
				frame.copyTo(temp);
				savedImages.push_back(temp);
			}
			break;
		case 13:
			//enter button
			//calibration
			if (savedImages.size() > 15)
			{
				camera_calibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);
				saveCameraCalibration("Calibration", cameraMatrix, distanceCoefficients);
			}
			break;
		case 27:
			//esc button
			//exit
			return;
		}
	}
}