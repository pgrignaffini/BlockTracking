#include "stdafx.h"
#include <librealsense2/rs.hpp>
#include <thread>

#include "BlockDetector.h"
#include "Note.h"
#include "Variable.h"
#include "Function.h"
#include "Board.h"
#include "trainedBlocks.h"
#include "ConfigurationManager.h"

//if we would like to calibrate our filter values, set to true.
bool calibrationMode = false;

struct MouseParams 
{
	unordered_map<int, trainedBlock*>* blocks; 
	cv::Point2f br;
	cv::Mat thresh_cycles;
};

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int thresh = 100;
cv::RNG rng(12345);
//default capture width and height
const int FRAME_WIDTH = 1280;
const int FRAME_HEIGHT = 720;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const double MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;

//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

const int frame_to_skip = 20;  // Number of frames to skip


void on_trackbar(int, void*)
{
	//This function gets called whenever a
	// trackbar position is changed
}

string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

cv::Rect getROI(Board plate)
{
	int x = plate.getTop_left_corner().x;
	int y = plate.getTop_left_corner().y;
	int width_roi = plate.getBottom_right_corner().x - x;
	int height_roi = y - plate.getBottom_right_corner().y;

	cv::Rect ROI = cv::Rect(x, y, width_roi, height_roi);
	return ROI;
}

trainedBlock* searchBlock(unordered_map<int, trainedBlock*>* blocks, int x, int y)
{
	trainedBlock* found = nullptr;

	for (auto it : *blocks)
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

void CallBackBlocks(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_RBUTTONDOWN)
	{
		std::cout << "Callback called" << endl;
		MouseParams* mp = static_cast<MouseParams*>(userdata);
		
		trainedBlock* clicked = searchBlock(mp->blocks, x, y); //returns clicked block

		if (clicked != nullptr)
		{
			try 
			{
				clicked->findNotes(mp->br, *mp->blocks);
				clicked->countCycles(mp->thresh_cycles);
				std::thread t(&trainedBlock::play, clicked); //run in a different thread
				t.detach();
			}

			catch (std::exception & exc)
			{
				cerr << exc.what() << endl;
			}
		}
	}
}

void createTrackbars()
{
	//create window for trackbars
	cv::namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf_s(TrackbarName, "H_MIN", H_MIN);
	sprintf_s(TrackbarName, "H_MAX", H_MAX);
	sprintf_s(TrackbarName, "S_MIN", S_MIN);
	sprintf_s(TrackbarName, "S_MAX", S_MAX);
	sprintf_s(TrackbarName, "V_MIN", V_MIN);
	sprintf_s(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	cv::createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	cv::createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	cv::createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	cv::createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	cv::createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	cv::createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

void printObject(Block block)
{
	std::cout << "Block " << block.getID() << ": " << block.getType() << std::endl;
	std::cout << "X: " << block.getXPos() << std::endl;
	std::cout << "Y: " << block.getYPos() << std::endl;
}

void printObjects(trainedBlocks* blocks)
{
	trainedBlock* block;
	unordered_map<int, trainedBlock*> map = blocks->getTrainedBlocks();

	for (auto& it : map)
	{
		block = it.second;
		std::cout << "Block " << block->getID() << ": " << block->getType() << std::endl;
	}
	
}

void drawObject(trainedBlocks* blocks, cv::Mat &frame)
{
	unordered_map<int, trainedBlock*> map = blocks->getTrainedBlocks();
	int xpos, ypos;

	for (auto& it : map)
	{
		xpos = it.second->getXPos();
		ypos = it.second->getYPos();

		circle(frame, cv::Point(xpos, ypos), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(xpos) + "," + intToString(ypos), cv::Point(xpos - 30, ypos + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
		cv::putText(frame, it.second->getType(), cv::Point(xpos, ypos - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
		cv::putText(frame, intToString(it.second->getID()), cv::Point(xpos, ypos - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
		//printObject(blocks.at(i));
	}
}

void drawObject(vector<Block> blocks, cv::Mat &frame)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		circle(frame, cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos()), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(blocks.at(i).getXPos()) + "," + intToString(blocks.at(i).getYPos()), cv::Point(blocks.at(i).getXPos() - 30, blocks.at(i).getYPos() + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
		cv::putText(frame, blocks.at(i).getType(), cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos() - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0));
		cv::putText(frame, intToString(blocks.at(i).getID()), cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos() - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
		//printObject(blocks.at(i));
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


trainedBlocks* trackFilteredObject(cv::Mat threshold, cv::Mat HSV, cv::Mat &cameraFeed, BlockDetector* detector, ConfigurationManager* config, trainedBlocks* trainedBlocks)
{
	trainedBlock* aBlock = new trainedBlock();
	cv::Mat temp, canny_out;
	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distanceCoefficients;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	vector<int> alive;

	detector->findCorners(cameraFeed); 

	//Canny(temp, canny_out, thresh, thresh * 2, 3);
	Canny(temp, canny_out, 0, 50, 5);

	//find contours of filtered image using openCV findContours function
	findContours(canny_out, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//imshow("Canny", canny_out);
	//imshow("Canny_filled", canny_out);

	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size();
		//cout << "Objects detected: " << numObjects << endl;
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS)
		{
			int index = 0;
			for (; index >= 0; index = hierarchy[index][0])
			{

				cv::Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.

				if (area > MIN_OBJECT_AREA)
				{
					try
					{

						int X = moment.m10 / area;
						int Y = moment.m01 / area;

						//returns the current block with the identifier
						//if it already exists returns a pointer to that block
						//if it doesn't already exist it inserts it in trainedBlocks
						//the function also updates the current block position and trains the block
						aBlock = trainedBlocks->update(detector, config, X, Y, contours, index);

						if (aBlock == nullptr) return trainedBlocks;
						
						objectFound = true;
					}
					catch (exception& e)
					{
						std::cout << "Something went wrong: " << e.what() << '\n';
					}
				}

				else objectFound = false;
			}
		}
	}

	/*
	/// Draw contours
	Mat drawing = Mat::zeros(canny_out.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); ++i)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	}*/

	/// Show in a window
	//namedWindow("Contours", WINDOW_AUTOSIZE);
	//imshow("Contours", drawing);

	//erase blocks no longer on board
	detector->check_for_changes(trainedBlocks->tBlocks);


	cout << "Identified Blocks: ";
	for (auto& it : trainedBlocks->tBlocks) cout << it.first << " ";
	cout << endl;

	if(!objectFound) putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", cv::Point(0, 50), 1, 2, cv::Scalar(0, 0, 255), 2);

	return trainedBlocks; 
}



int main(int argc, char* argv[])
{
	ConfigurationManager* conf = new ConfigurationManager("conf/pianoConf.txt", "conf/deps.txt");
	conf->loadConf();
	conf->loadDeps();

	rs2::context rsCtx;
	auto list = rsCtx.query_devices();
	rs2::device rsCam = list.front();

	rs2::pipeline pipe;
	rs2::pipeline_profile selection = pipe.start();

	cv::Scalar HSVminBoard = cv::Scalar(44, 0, 0); //gray
	cv::Scalar HSVmaxBoard = cv::Scalar(256, 40, 256); //gray

	//cv::Scalar HSVminBoard = cv::Scalar(54, 58, 26); //green
	//cv::Scalar HSVmaxBoard = cv::Scalar(100, 256, 256); //green

	//cv::Scalar HSVminPlus = cv::Scalar(0, 133, 163); //red
	//cv::Scalar HSVmaxPlus = cv::Scalar(256, 256, 256); //red

	//cv::Scalar HSVminPlus = cv::Scalar(0, 0, 55); //black
	//cv::Scalar HSVmaxPlus = cv::Scalar(256, 256, 256); //black 

	cv::Scalar HSVminPlus = cv::Scalar(0, 0, 0); //black
	cv::Scalar HSVmaxPlus = cv::Scalar(256, 256, 50); //black 

	//orange color is hardcoded in the Block structure

	//Matrix to store each frame of the webcam feed
	cv::Mat distanceCoefficients, HSV, gray, board;
	cv::Mat threshold_block, threshold_plus, threshold_board, adaptive_threshold_block;
	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);

	vector<trainedBlock> trackedBlocks;
	vector<vector<cv::Point2f>> markerCorners;
	vector<int> IDs;

	BlockDetector* detector = new BlockDetector();
	Board plate;
	trainedBlocks* trainBlocks = new trainedBlocks();

	if (calibrationMode)
	{
		//create slider bars for HSV filtering
		createTrackbars();
	}

	//findMarkers.loadCameraCalibration("CalibrationBlaster", cameraMatrix, distanceCoefficients);
	//detector->loadCameraCalibration("CalibrationIntel", cameraMatrix, distanceCoefficients);
	cv::Ptr<cv::aruco::Dictionary> markerDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);


	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while (true) {

		int counter = 0;

		cv::Mat cameraFeed, bgr;
		auto rgbStream = selection.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
		auto height = rgbStream.height();
		auto width = rgbStream.width();
		//	cout << height << width << endl;
		rs2::frameset frames = pipe.wait_for_frames();

		if (frames.size() > 0) 
		{
			rs2::frame rgbFrame = frames.first(RS2_STREAM_COLOR); 
			bgr = cv::Mat(height, width, CV_8UC3, (void*)rgbFrame.get_data());
			//Mat cameraFeed;
			cvtColor(bgr, cameraFeed, cv::COLOR_BGR2RGB);
			//cvtColor(bgr, src, COLOR_BGR2RGB);
			//imshow("Test", cameraFeed);
			if (cv::waitKey(1000 / 60) >= 0) break;
		}

		//set ROI

		//Rect ROI = Rect(120, 70, 365, 355);
		//Mat image_roi = cameraFeed(ROI);
		//cameraFeed = image_roi;
		//imshow("ROI", image_roi);

		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, cv::COLOR_BGR2HSV);
		//cvtColor(src, hsv, COLOR_BGR2HSV);
		//convert frame from BGR to GRAY colorspace
		cvtColor(cameraFeed, gray, cv::COLOR_BGR2GRAY);


		if (calibrationMode)
		{
			//if in calibration mode, we track objects based on the HSV slider values.
			//cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, cv::Scalar(H_MIN, S_MIN, V_MIN), cv::Scalar(H_MAX, S_MAX, V_MAX), threshold_block);
			inRange(HSV, HSVminPlus, HSVmaxPlus, threshold_plus);
			inRange(HSV, HSVminBoard, HSVmaxBoard, threshold_board); // board

			morphOps(threshold_block);
			morphOps(threshold_board);

			imshow(windowName2, threshold_block);
			//trackFilteredObject(threshold_block, HSV, cameraFeed);
		}

		else
		{
			trainedBlock* block = new trainedBlock();

			//cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, block->getHSVmin(), block->getHSVmax(), threshold_block); // orange blocks
			inRange(HSV, HSVminPlus, HSVmaxPlus, threshold_plus); //black blocks
			inRange(HSV, HSVminBoard, HSVmaxBoard, threshold_board); // board

			morphOps(threshold_block);
			morphOps(threshold_board);
			//no need for morphOps on threshold_plus because we're only interested on the number of plus blocks
			//not on shape or contours

			plate.identifyBoard(threshold_board);

			if (counter == 0) trainBlocks = trackFilteredObject(threshold_block, HSV, cameraFeed, detector, conf, trainBlocks);
			counter = (++counter)%25;
			drawObject(trainBlocks, cameraFeed);

		}

		///Show frames 
		//imshow(windowName2,threshold_block);
		//imshow("Plus threshold", threshold_plus);
		//imshow("Board threshold", threshold_board);

		//Mouse params for callback
		MouseParams* mp = new MouseParams();
		mp->blocks = &trainBlocks->tBlocks;
		mp->br = plate.getBottom_right_corner();
		mp->thresh_cycles = threshold_plus; 

		cv::setMouseCallback(windowName, CallBackBlocks, static_cast<void*>(mp));

		imshow(windowName, cameraFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		cv::waitKey(30);
	}

	return 0;
}


