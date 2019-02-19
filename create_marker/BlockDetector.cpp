#include "stdafx.h"
#include "BlockDetector.h"

using namespace std;
using namespace cv;

BlockDetector::BlockDetector()
{
	calibrationSquareDimension = 0.026f; //meters = 2.6cm
	arucoSquareDimension = 0.015f; // 1.5cm
	chessboardDimensions = cv::Size(6, 9); //number of intersection points on the chessboard
}

BlockDetector::~BlockDetector()
{
}

//Getters

float BlockDetector::getCalibrationSquareDimension()
{
	return BlockDetector::calibrationSquareDimension;
}

float BlockDetector::getArucoSquareDimension()
{
	return BlockDetector::arucoSquareDimension;
}

cv::Size BlockDetector::getChessboardDimensions()
{
	return BlockDetector::chessboardDimensions;
}

std::vector<int> BlockDetector::getIDs()
{
	return BlockDetector::IDs;
}

vector<vector<Point2f>> BlockDetector::getIdentifiedCorners()
{
	return BlockDetector::markerCorners;
}


//Setters

void BlockDetector::setCalibrationSquareDimension(float calib)
{
	BlockDetector::calibrationSquareDimension = calib;
}

void BlockDetector::setArucoSquareDimension(float aruc)
{
	BlockDetector::arucoSquareDimension = aruc;
}

void BlockDetector::setChessboardDimensions(cv::Size size)
{
	BlockDetector::chessboardDimensions = size;
}

void BlockDetector::setIDs(vector<int> ids)
{
	BlockDetector::IDs = ids;
}


void BlockDetector::setCorners(std::vector<std::vector<cv::Point2f>> corners)
{
	BlockDetector::markerCorners = corners;
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

void BlockDetector::createArucoMarkers()
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

void BlockDetector::createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners)
{
	for (int i = 0; i < boardSize.height; i++)
	{
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
		}
	}
}

void BlockDetector::getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, bool showResults)
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

void BlockDetector::camera_calibration(std::vector<cv::Mat> calibrationImages, cv::Size boardSize, float squareEdgeLength, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
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

bool BlockDetector::saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients)
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

bool BlockDetector::loadCameraCalibration(std::string name, cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
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

void BlockDetector::cameraCalibrationProcess(cv::Mat & cameraMatrix, cv::Mat & distanceCoefficients)
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
	aruco::estimatePoseSingleMarkers(corners, getArucoSquareDimension(), cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
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