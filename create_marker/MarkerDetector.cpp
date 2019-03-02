#include "stdafx.h"
#include "MarkerDetector.h"

#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\aruco.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

const int MARKER_DIMENSION = 35;

MarkerDetector::MarkerDetector()
{
	calibrationSquareDimension = 0.039f; //meters = 3.9cm
	arucoSquareDimension = 0.01998f; // 1.9cm
	chessboardDimensions = cv::Size(6, 9); //number of intersection points on the chessboard
}

MarkerDetector::~MarkerDetector()
{
}

float MarkerDetector::getCalibrationSquareDimension()
{
	return MarkerDetector::calibrationSquareDimension;
}

float MarkerDetector::getArucoSquareDimension()
{
	return MarkerDetector::arucoSquareDimension;
}

cv::Size MarkerDetector::getChessboardDimensions()
{
	return MarkerDetector::chessboardDimensions;
}

std::vector<int> MarkerDetector::getIDs()
{
	return MarkerDetector::IDs;
}

void MarkerDetector::setCalibrationSquareDimension(float calib)
{
	MarkerDetector::calibrationSquareDimension = calib;
}

void MarkerDetector::setArucoSquareDimension(float aruc)
{
	MarkerDetector::arucoSquareDimension = aruc;
}

void MarkerDetector::setChessboardDimensions(cv::Size size)
{
	MarkerDetector::chessboardDimensions = size;
}

void MarkerDetector::setIDs(vector<int> ids)
{
	MarkerDetector::IDs = ids;
}

void MarkerDetector::createArucoMarkers()
{
	Mat outputMarker;
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
	//create markers and print them in a file
	// 1 pixel = 0.26 mm
	//square block = 2cm => 75 pixel
	for (int i = 0; i < 50; i++)
	{
		aruco::drawMarker(markerDictionary, i, 500, outputMarker, 1);
		ostringstream convert;
		string imageName = "4x4Marker_";
		convert << imageName << i << ".jpg";
		imwrite(convert.str(), outputMarker);
	}
}

void MarkerDetector::createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners)
{
	for (int i = 0; i < boardSize.height; i++)
	{
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
		}
	}
}

void MarkerDetector::getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults)
{
	for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++)
	{
		vector<Point2f> pointBuf; //buffer to store all the corners
		bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

		if (found)	allFoundCorners.push_back(pointBuf);

		if (showResults)
		{
			drawChessboardCorners(*iter, Size(6, 9), pointBuf, found);
			imshow("Looking for corners", *iter);
			waitKey(0);
		}
	}
}

void MarkerDetector::camera_calibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	vector<vector<Point2f>> checkerboardImageSpacePoints;
	getChessboardCorners(calibrationImages, checkerboardImageSpacePoints, false);

	vector<vector<Point3f>> worldSpaceCornerPoints(1);

	createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);

	worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

	vector<Mat> rVectors, tVectors;
	distanceCoefficients = Mat::zeros(8, 1, CV_64F);

	calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
}

bool MarkerDetector::saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients)
{
	ofstream outStream(name);
	if (outStream)
	{
		uint16_t rows = cameraMatrix.rows;
		uint16_t columns = cameraMatrix.cols;

		outStream << rows << endl;
		outStream << columns << endl;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double value = cameraMatrix.at<double>(r, c);
				outStream << value << endl;
			}
		}

		rows = distanceCoefficients.rows;
		columns = distanceCoefficients.cols;

		outStream << rows << endl;
		outStream << columns << endl;

		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < columns; c++)
			{
				double value = distanceCoefficients.at<double>(r, c);
				outStream << value << endl;
			}
		}

		outStream.close();
		return true;
	}

	return false;
}

bool MarkerDetector::loadCameraCalibration(std::string name, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	ifstream instream(name);
	if (instream)
	{
		uint16_t rows;
		uint16_t columns;

		instream >> rows;
		instream >> columns;

		cameraMatrix = Mat(Size(columns, rows), CV_64F);

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

		distanceCoefficients = Mat::zeros(rows, columns, CV_64F);

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

void MarkerDetector::cameraCalibrationProcess(cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
{
	Mat frame;
	Mat drawToFrame;
	vector<Mat> savedImages;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	VideoCapture vid(0);

	if (!vid.isOpened()) return;

	int framesPerSecond = 40;
	namedWindow("Webcam", WINDOW_AUTOSIZE);

	while (true)
	{
		if (!vid.read(frame)) break;

		vector<Vec2f> foundPoints;
		bool found = false;

		//found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
		found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
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

		char character = waitKey(1000 / framesPerSecond); //waitKey returns the pressed key

		switch (character)
		{
		case ' ':
			//saving image
			if (found)
			{
				Mat temp;
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

vector<vector<Point2f>> MarkerDetector::findCorners(Mat cameraFeed, MarkerDetector& detective)
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
	aruco::estimatePoseSingleMarkers(corners, detective.getArucoSquareDimension(), cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
	//aruco::drawDetectedMarkers(cameraFeed, corners, markerIdentifiers); //show IDs

	detective.setIDs(markerIdentifiers);

	return corners;
}

//this function identifies the right marker ID to assign to the block in consideration, the point <xm,ym> represents its center
int MarkerDetector::findId(int xm, int ym, vector<vector<Point2f>> identifiedCorners)
{
	int idFound = -1;

	for (int i = 0; i < identifiedCorners.size(); i++)
	{
		Rect markerPosition = boundingRect(identifiedCorners[i]);
		//cout << "Width: " << markerPosition.width << " Height: " << markerPosition.height << endl;
		Rect blockPosition = Rect(xm, ym, MARKER_DIMENSION, MARKER_DIMENSION);
		//if the block position matches the marker position it means that the marker belongs to the block taken into consideration
		Rect intersection = markerPosition & blockPosition;
		if (intersection.area() > 0) //rectangles intersection
		{
			idFound = i;
		}
	}

	if (idFound != -1) return IDs[idFound];
	return idFound;
}
