#pragma once
#include "stdafx.h"
#include "trainedBlock.h"
#include "Function.h"
#include "Board.h"
#include "ConfigurationManager.h"
#include "BlockDetector.h"
#include "ColorCalibration.h"
#include "Token.h"
#include "Hand.h"
#include <random>

//if we would like to calibrate our filter values, set to true.
bool calibrationMode = false;
//number of channels 
const int N_CHANNEL = 32;

//random channel number generator
std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::uniform_int_distribution<int>  distr(0, N_CHANNEL);

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

void playInParallel(std::vector<trainedBlock*> inParallel)
{
	//play in parallel
	#pragma omp parallel for
	for (int i = 0; i < inParallel.size(); i++)
	{
		cout << "Playing parallel " << endl;
		cout << inParallel.at(i)->getID() << " ";
		inParallel.at(i)->play(-1);
	}
	
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
				int channel = distr(generator); //returns random number between [0...N_CHANNEL[
				while (Mix_Playing(channel))
				{
					channel++; //make sure the channel is not already in use
					if (channel >= N_CHANNEL) channel = 0;
				}

				clicked->findNotes(mp->br, mp->blocks);
				
				if (clicked->looping)
				{
					std::thread t(&trainedBlock::play, clicked, channel);
					t.detach();
				}
				else clicked->play(channel);
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

		std::thread t(playInParallel, inParallel);
		t.detach();

	}

	else if (event == cv::EVENT_MBUTTONDOWN)
	{
		Mix_HaltChannel(-1); //halts all channels
	}
}



void PlayWithGestures(Hand* hand, Board* board, unordered_map<int, trainedBlock*> blocks)
{	
	std::cout << "Finger detected" << endl;
	trainedBlock* clicked = nullptr;
	cv::Point blockPos;
	cv::Point br = board->getBottom_right_corner();
	cv::Rect bline = board->getBottomLine();
	cv::Rect tipArea = hand->getTipArea();
	cv::Point fingerTip = hand->getFinger();

	if (bline.contains(fingerTip))
	{
		Mix_HaltChannel(-1); //halts all channels

		std::vector<trainedBlock*> inParallel = getParallelBlocks(blocks);

		//play in parallel
		#pragma omp parallel for
		for (int i = 0; i < inParallel.size(); i++)
		{
			cout << "Playing parallel " << endl;
			cout << inParallel.at(i)->getID() << " ";
			inParallel.at(i)->play(i);
		}
	}

	else
	{
		for (auto it : blocks)
		{
			blockPos = cv::Point(it.second->getXPos(), it.second->getYPos());
			if (tipArea.contains(blockPos))
			{
				clicked = it.second;
				break;
			}
		}

		if (clicked != nullptr)
		{
			try
			{
				int channel = distr(generator);
				while (Mix_Playing(channel))
				{
					channel++; //make sure the channel is not already in use
					if (channel >= N_CHANNEL) channel = 0;
				}

				clicked->findNotes(br, blocks);
				
				if (clicked->looping)
				{
					std::thread t(&trainedBlock::play, clicked, channel);
					t.detach();
				}
				else clicked->play(channel);
			}

			catch (std::exception & exc)
			{
				cerr << exc.what() << endl;
			}
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

	for (auto it : tBlocks)
	{
		xpos = it.second->getXPos();
		ypos = it.second->getYPos();

		circle(frame, cv::Point(xpos, ypos), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(xpos) + "," + intToString(ypos), cv::Point(xpos - 30, ypos + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
		cv::putText(frame, it.second->getType(), cv::Point(xpos, ypos - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
		cv::putText(frame, intToString(it.second->getID()), cv::Point(xpos, ypos - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
		//cv::putText(frame, BoolToString(it.second->isLastLine()), cv::Point(xpos, ypos - 40), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
		//it.second->printRange(frame);
	}
}

void drawTokens(std::vector<Token*> tokens, cv::Mat &frame)
{
	int xpos, ypos;
	bool lastLine;

	for (auto it : tokens)
	{
		xpos = it->getXPos();
		ypos = it->getYPos();

		circle(frame, cv::Point(xpos, ypos), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(xpos) + "," + intToString(ypos), cv::Point(xpos - 30, ypos + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
	}
}

void drawFinger(Hand* hand, cv::Mat &frame)
{
	cv::Point fingerTip = hand->getFinger();
	int x_offset, y_offset;
	cv::Rect tipArea = hand->getTipArea(); 

	if (fingerTip != cv::Point(0,0) && hand->isDetected())
	{
		cv::circle(frame, fingerTip, 8, cv::Scalar(0, 255, 0), 2);
		
		if (hand->getDirection() == 1)
		{
			cv::putText(frame, "Vertical", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0));
		}

		if (hand->getDirection() == 2)
		{
			cv::putText(frame, "Horizontal", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0));
		}

		cv::rectangle(frame, tipArea, cv::Scalar(0, 0, 255), 4);
	}
}


void morphOps(cv::Mat &thresh, int n_erodes, int n_dilates)
{
	for (int i = 0; i < n_erodes; i++) erode(thresh, thresh, cv::Mat());
	for (int i = 0; i < n_dilates; i++) dilate(thresh, thresh, cv::Mat());
}

int main(int argc, char* argv[])
{
	//create slider bars for HSV filtering
	ColorCalibration* colorCalib = new ColorCalibration();
	if (calibrationMode)
	{
		colorCalib->createTrackbars();
	}

	ConfigurationManager* conf = new ConfigurationManager("conf/beatConf.txt", "conf/deps.txt");
	conf->loadConf();
	conf->loadDeps();

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		// Error message if can't initialize
	}

	// Amount of channels (Max amount of sounds playing at the same time)
	Mix_AllocateChannels(N_CHANNEL);


	rs2::context rsCtx;
	auto list = rsCtx.query_devices();
	rs2::device rsCam = list.front();
	rs2::pipeline pipe = rs2::pipeline();
	rs2::pipeline_profile selection = pipe.start();

	//Matrix to store each frame of the webcam feed
	cv::Mat distanceCoefficients, HSV;
	cv::Mat threshold, threshold_block, threshold_token, threshold_board, threshold_hand;
	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

	Token* token = new Token();
	Board* board = new Board();
	BlockDetector* detector = new BlockDetector();
	CameraCalibration* camCalib = new CameraCalibration();
	Hand* hand = new Hand();

	///camera calibration
	//camCalib->cameraCalibrationProcess(cameraMatrix, distanceCoefficients);
	//return 0;

	//trainBlocks is the structure that contains all the blocks on the board
	unordered_map<int, trainedBlock*> trainBlocks;

	//tokens
	std::vector<Token*> tokens;

	//utilities for camera calibration
	camCalib->loadCameraCalibration("IntelCalibration", cameraMatrix, distanceCoefficients);

	int counter = 0;
	
	//all operations will be performed within this loop
	while (true) 
	{
		
		cv::Mat cameraFeed, cameraFeedT, bgr;
		auto rgbStream = selection.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
		auto height = rgbStream.height();
		auto width = rgbStream.width();
		//cout << height << width << endl;
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
			cv::inRange(HSV, cv::Scalar(colorCalib->H_MIN, colorCalib->S_MIN, colorCalib->V_MIN),
							 cv::Scalar(colorCalib->H_MAX, colorCalib->S_MAX, colorCalib->V_MAX), threshold);

			cv::imshow("Threshold", threshold);	
		}

		else
		{
			trainedBlock* block = new trainedBlock();

			inRange(HSV, block->getHSVmin(), block->getHSVmax(), threshold_block); // blocks
			inRange(HSV, token->getHSVmin(), token->getHSVmax(), threshold_token); // tokens
			inRange(HSV, board->getHSVmin(), board->getHSVmax(), threshold_board); // board
			inRange(HSV, hand->getHSVmin(),  hand->getHSVmax(),  threshold_hand); // hand

			morphOps(threshold_block, 1, 3);
			morphOps(threshold_board, 1, 3);
			morphOps(threshold_token, 3, 4);


			if (counter == 0)
			{
				board->identifyBoard(threshold_board);
				detector->setBLine(board->getBottomLine());
			}
			
			if (counter == 2)
			{
				detector->trackFilteredObject(threshold_block, cameraFeed, cameraMatrix, distanceCoefficients, conf, trainBlocks);
			}

			if (counter == 4)
			{
				tokens = detector->trackTokens(threshold_token);
				detector->setUpFunctions(tokens, trainBlocks);
			}

			if (counter == 6)
			{
				detector->updateBlocks(trainBlocks);
			}

			counter = (++counter) % 10;

			std::cout << "Tokens: " << tokens.size() << std::endl;

			//hand->findFinger(threshold_hand);
			//hand->calculateTipArea();

			drawObject(trainBlocks, cameraFeed);
			//drawFinger(hand, cameraFeed);
			drawTokens(tokens, cameraFeed);
		
			///Show frames 
			imshow("Block threshold", threshold_block);
			imshow("Token threshold", threshold_token);
			imshow("Board threshold", threshold_board);
			//imshow("Hand threshold",  threshold_hand);
			///display window with board
			//printBoard(threshold_board);

		}


		if (hand->isDetected()) 
			PlayWithGestures(hand, board, trainBlocks);

		///Mouse params for callback
		MouseParams* mp = new MouseParams();
		mp->blocks = trainBlocks;
		mp->br = board->getBottom_right_corner();
		mp->thresh_cycles = threshold_token; 


		cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
		cv::setMouseCallback("Camera", CallBackBlocks, static_cast<void*>(mp));
		cv::rectangle(cameraFeed, board->getBottomLine(), cv::Scalar(0, 0, 255), 4);

		imshow("Camera", cameraFeed);


		cout << "Identified Blocks: ";
		for (auto it : trainBlocks) cout << it.first << " ";
		cout << endl;

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		cv::waitKey(30);
	}

	return 0;
}


