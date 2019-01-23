#include "stdafx.h"

#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\aruco.hpp>
#include <opencv2\calib3d.hpp>
#include <opencv2\highgui.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

const float calibrationSquareDimension = 0.026f; //meters = 2.6cm
const float arucoSquareDimension = 0.015f; // 1.5cm
const Size chessboardDimensions = Size(6, 9); //number of intersection points on the chessboard

//Use Qt for GUI

void createArucoMarkers()
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

void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners)
{
	for (int i = 0; i < boardSize.height; i++)
	{
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
		}
	}
}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false) // find and visualize chessboard corners
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

void camera_calibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients)
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

bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients)
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

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients)
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
				cout << cameraMatrix.at<double>(r, c) << "\n";
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
				cout << distanceCoefficients.at<double>(r, c) << "\n";
			}
		}

		instream.close();
		return true;

	}

	return false;

}

int startWebcamMonitoring(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimensions)
{
	Mat frame;
	vector<int> markerIDs;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	aruco::DetectorParameters parameters;
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	VideoCapture vid(0);
	if (!vid.isOpened())
	{
		return -1;
	}

	namedWindow("Find markers", WINDOW_AUTOSIZE);

	vector<Vec3d> rotationVectors, translationVectors;

	while (true)
	{
		if (!vid.read(frame)) break;

		aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIDs);
		aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimensions, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
		aruco::drawDetectedMarkers(frame, markerCorners, markerIDs); //show IDs

		for (int i = 0; i < markerIDs.size(); i++)
		{
		//	aruco::drawAxis(frame, cameraMatrix, distanceCoefficients, rotationVectors[i], translationVectors[i], 0.1f); // show axis
			cout << "Detected block: " << markerIDs[i] << endl;
		}

		imshow("Webcam", frame);
		if (waitKey(30) >= 0) break;

	}

	return 1;

}

void cameraCalibrationProcess(Mat& cameraMatrix, Mat& distanceCoefficients)
{
	Mat frame;
	Mat drawToFrame;
	vector<Mat> savedImages;
	vector<vector<Point2f>> markerCorners, rejectedCandidates;

	VideoCapture vid(2);

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
				saveCameraCalibration("CalibrationIntel", cameraMatrix, distanceCoefficients);
			}
			break;
		case 27:
			//esc button
			//exit
			return;
		}
	}
}
/*
int main()

{
	//createArucoMarkers();
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;

	cameraCalibrationProcess(cameraMatrix, distanceCoefficients);
	//loadCameraCalibration("CalibrationIntel", cameraMatrix, distanceCoefficients);
	startWebcamMonitoring(cameraMatrix, distanceCoefficients, 0.099f);
	return 0;
}
*/
