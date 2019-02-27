#pragma once
#include "stdafx.h"
#include "trainedBlock.h"
#include "Function.h"
#include "Board.h"
#include "ConfigurationManager.h"
#include "BlockDetector.h"
#include "ColorCalibration.h"
#include "Token.h"

//if we would like to calibrate our filter values, set to true.
bool calibrationMode = false;

struct MouseParams 
{
	unordered_map<int, trainedBlock*> blocks; 
	cv::Point2f br;
	cv::Mat thresh_cycles;
};

inline const string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

inline const char * const BoolToString(bool b)
{
	return b ? "true" : "false";
}

trainedBlock* searchBlock(unordered_map<int, trainedBlock*>& blocks, int x, int y)
{
	trainedBlock* found = nullptr;

	for (auto it : blocks)
	{
		cv::Point2f mouseClick = cv::Point2f(x, y);
		cv::Point2f blockCenter = cv::Point2f(it.second->getXPos(), it.second->getYPos());
		cv::Rect2f blockArea = cv::Rect2f(blockCenter.x - BLOCK_WIDTH / 2, blockCenter.y - BLOCK_HEIGHT / 2, BLOCK_WIDTH, BLOCK_HEIGHT);
		
		if (blockArea.contains(mouseClick))
		{
			found = it.second;
			break;
		}
	}
	
	return found;
}

std::vector<trainedBlock*> getParallelBlocks(unordered_map<int, trainedBlock*>& blocks)
{
	std::vector<trainedBlock*> inParallel;

	for (auto it : blocks)
	{
		if (it.second->isLastLine())
		{
			inParallel.push_back(it.second);
		}
	}

	return inParallel; 
}

void CallBackBlocks(int event, int x, int y, int flags, void* userdata)
{
	MouseParams* mp = static_cast<MouseParams*>(userdata);

	if (event == cv::EVENT_RBUTTONDOWN)
	{
		std::cout << "RBUTTON clicked" << endl;
		trainedBlock* clicked = searchBlock(mp->blocks, x, y); //returns clicked block

		if (clicked != nullptr)
		{
			try 
			{
				clicked->findNotes(mp->br, mp->blocks);
				clicked->countCycles(mp->thresh_cycles);
				clicked->play(-1);
			}

			catch (std::exception & exc)
			{
				cerr << exc.what() << endl;
			}
		}
	}

	else if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << "LBUTTON clicked" << endl;
		Mix_HaltChannel(-1); //halts all channels

		std::vector<trainedBlock*> inParallel = getParallelBlocks(mp->blocks);

		#pragma omp parallel for
		for (int i = 0; i < inParallel.size(); i++)
		{
			inParallel.at(i)->play(i);
		}
		
	}
}

void printObjects(unordered_map<int, trainedBlock*> blocks)
{
	trainedBlock* block;

	for (auto& it : blocks)
	{
		block = it.second;
		std::cout << "Block " << block->getID() << ": " << block->getType() << std::endl;
	}
	
}

void drawObject(unordered_map<int, trainedBlock*> tBlocks, cv::Mat &frame)
{
	int xpos, ypos;
	bool lastLine;

	for (auto& it : tBlocks)
	{
		xpos = it.second->getXPos();
		ypos = it.second->getYPos();

		circle(frame, cv::Point(xpos, ypos), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(xpos) + "," + intToString(ypos), cv::Point(xpos - 30, ypos + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
		cv::putText(frame, it.second->getType(), cv::Point(xpos, ypos - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
		cv::putText(frame, intToString(it.second->getID()), cv::Point(xpos, ypos - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
		cv::putText(frame, BoolToString(it.second->isLastLine()), cv::Point(xpos, ypos - 40), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
	}
}


void morphOps(cv::Mat &thresh)
{
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//dilate with larger element so make sure object is visible
	cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);

	cv::Mat erodeElement2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::Mat dilateElement2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	erode(thresh, thresh, erodeElement2);
	erode(thresh, thresh, erodeElement2);

	dilate(thresh, thresh, dilateElement2);
	dilate(thresh, thresh, dilateElement2);
}

int main(int argc, char* argv[])
{
	//create slider bars for HSV filtering
	ColorCalibration* colorcalib = new ColorCalibration();
	if (calibrationMode)
	{
		colorcalib->createTrackbars();
	}

	ConfigurationManager* conf = new ConfigurationManager("conf/pianoConf.txt", "conf/deps.txt");
	conf->loadConf();
	conf->loadDeps();

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		// Error message if can't initialize
	}

	// Amount of channels (Max amount of sounds playing at the same time)
	Mix_AllocateChannels(32);

	rs2::context rsCtx;
	auto list = rsCtx.query_devices();
	rs2::device rsCam = list.front();
	rs2::pipeline pipe;
	rs2::pipeline_profile selection = pipe.start();

	//Matrix to store each frame of the webcam feed
	cv::Mat distanceCoefficients, HSV;
	cv::Mat threshold_block, threshold_token, threshold_board;
	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

	Token* token = new Token();
	Board* board = new Board();
	BlockDetector* detector = new BlockDetector();

	//trainBlocks is the structure that contains all the blocks on the board
	unordered_map<int, trainedBlock*> trainBlocks;

	//utilities for camera calibration
	//findMarkers.loadCameraCalibration("CalibrationBlaster", cameraMatrix, distanceCoefficients); 
	//detector->feed->loadCameraCalibration("CalibrationIntel", cameraMatrix, distanceCoefficients);
	
	cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

	//all operations will be performed within this loop
	while (true) 
	{
		int counter = 0;
		cv::Mat cameraFeed, bgr;
		auto rgbStream = selection.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
		auto height = rgbStream.height();
		auto width = rgbStream.width();
		//	cout << height << width << endl;
		rs2::frameset frames = pipe.wait_for_frames();

		if (frames.size() > 0) 
		{
			//acquire frames from camera
			rs2::frame rgbFrame = frames.first(RS2_STREAM_COLOR); 
			bgr = cv::Mat(height, width, CV_8UC3, (void*)rgbFrame.get_data());
			cvtColor(bgr, cameraFeed, cv::COLOR_BGR2RGB);
			if (cv::waitKey(1000 / 60) >= 0) break;
		}

		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, cv::COLOR_BGR2HSV);
		//imshow("HSV", HSV);

		if (calibrationMode)
		{
			trainedBlock* block = new trainedBlock();

			//if in calibration mode, we track objects based on the HSV slider values
			//this branch is implemented just for colors calibrating purposes
			cv::inRange(HSV, cv::Scalar(colorcalib->H_MIN, colorcalib->S_MIN, colorcalib->V_MIN),
							 cv::Scalar(colorcalib->H_MAX, colorcalib->S_MAX, colorcalib->V_MAX), threshold_block);

			inRange(HSV, token->getHSVmin(), token->getHSVmax(), threshold_token); //black blocks
			inRange(HSV, board->getHSVmin(), board->getHSVmax(), threshold_board); // board

			cv::imshow("Threshold Block", threshold_block);
			
		}

		else
		{
			trainedBlock* block = new trainedBlock();

			//cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, block->getHSVmin(), block->getHSVmax(), threshold_block); // orange blocks
			inRange(HSV, token->getHSVmin(), token->getHSVmax(), threshold_token); //black blocks
			inRange(HSV, board->getHSVmin(), board->getHSVmax(), threshold_board); // board

			morphOps(threshold_block);
			morphOps(threshold_board);
			//no need for morphOps on threshold_plus because we're only interested on the number of plus blocks
			//not on shape or contours

			board->identifyBoard(threshold_board);
			detector->setBLine(board->getBottomLine());

			if (counter == 0) detector->trackFilteredObject(threshold_block, cameraFeed, conf, trainBlocks);
			counter = (++counter) % 25;
			drawObject(trainBlocks, cameraFeed); 

		}

		///Show frames 
		//imshow("Block threshold", threshold_block);
		//imshow("Token threshold", threshold_token);
		//imshow("Board threshold", threshold_board);

		//Mouse params for callback
		MouseParams* mp = new MouseParams();
		mp->blocks = trainBlocks;
		mp->br = board->getBottom_right_corner();
		mp->thresh_cycles = threshold_token; 

		cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
		cv::setMouseCallback("Camera", CallBackBlocks, static_cast<void*>(mp));
		imshow("Camera", cameraFeed);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		cv::waitKey(30);
	}

	return 0;
}


