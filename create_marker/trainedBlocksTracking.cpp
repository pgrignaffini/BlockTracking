#include "stdafx.h"
#include <librealsense2/rs.hpp>

#include "BlockDetector.h"
#include "Note.h"
#include "Variable.h"
#include "Function.h"
#include "Board.h"
#include "trainedBlocks.h"

using namespace cv;

//if we would like to calibrate our filter values, set to true.
bool calibrationMode = false;

struct MouseParams
{
	unordered_map<int, trainedBlock*>* blocks; 
	Point2f br;
	Mat thresh_cycles;
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
RNG rng(12345);
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;

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

Rect getROI(Board plate)
{
	int x = plate.getTop_left_corner().x;
	int y = plate.getTop_left_corner().y;
	int width_roi = plate.getBottom_right_corner().x - x;
	int height_roi = y - plate.getBottom_right_corner().y;

	Rect ROI = Rect(x, y, width_roi, height_roi);
	return ROI;
}

trainedBlock* searchBlock(unordered_map<int, trainedBlock*>* blocks, int x, int y)
{
	trainedBlock* found = nullptr;

	for (auto it : *blocks)
	{
		Point2f mouseClick = Point2f(x, y);
		Point2f blockCenter = Point2f(it.second->getXPos(), it.second->getYPos());
		Rect2f blockArea = Rect2f(blockCenter.x - BLOCK_WIDTH / 2, blockCenter.y - BLOCK_HEIGHT / 2, BLOCK_WIDTH, BLOCK_HEIGHT);
		
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
	if (event == EVENT_RBUTTONDOWN)
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
				clicked->play();
			}

			catch (const char* msg)
			{
				cerr << msg << endl;
			}
		}
	}
}

void createTrackbars()
{
	//create window for trackbars
	namedWindow(trackbarWindowName, 0);
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
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

void printObject(Block block)
{
	cout << "Block " << block.getID() << ": " << block.getType() << endl;
	cout << "X: " << block.getXPos() << endl;
	cout << "Y: " << block.getYPos() << endl;
}

void printObjects(trainedBlocks* blocks)
{
	trainedBlock* block;
	unordered_map<int, trainedBlock*> map = blocks->getTrainedBlocks();

	for (auto& it : map)
	{
		block = it.second;
		cout << "Block " << block->getID() << ": " << block->getType() << endl;
	}
	
}

void drawObject(trainedBlocks* blocks, Mat &frame)
{
	unordered_map<int, trainedBlock*> map = blocks->getTrainedBlocks();
	int xpos, ypos;

	for (auto& it : map)
	{
		xpos = it.second->getXPos();
		ypos = it.second->getYPos();

		circle(frame, cv::Point(xpos, ypos), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(xpos) + "," + intToString(ypos), cv::Point(xpos - 30, ypos + 30), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 255));
		cv::putText(frame, it.second->getType(), cv::Point(xpos, ypos - 10), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
		cv::putText(frame, intToString(it.second->getID()), cv::Point(xpos, ypos - 20), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
		//printObject(blocks.at(i));
	}
}

void drawObject(vector<Block> blocks, Mat &frame)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		circle(frame, cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos()), 10, cv::Scalar(255, 0, 0));
		cv::putText(frame, intToString(blocks.at(i).getXPos()) + "," + intToString(blocks.at(i).getYPos()), cv::Point(blocks.at(i).getXPos() - 30, blocks.at(i).getYPos() + 30), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 255));
		cv::putText(frame, blocks.at(i).getType(), cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos() - 10), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
		cv::putText(frame, intToString(blocks.at(i).getID()), cv::Point(blocks.at(i).getXPos(), blocks.at(i).getYPos() - 20), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));
		//printObject(blocks.at(i));
	}
}

std::vector<cv::Point> contoursConvexHull(std::vector<cv::Point> contours)
{
	std::vector<cv::Point> result;
	std::vector<cv::Point> pts;
	for (size_t j = 0; j < contours.size(); j++)
		pts.push_back(contours[j]);
	cv::convexHull(pts, result);
	return result;
}

void morphOps(Mat &thresh)
{
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	//Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(5, 5));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);

	Mat erodeElement2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	Mat dilateElement2 = getStructuringElement(MORPH_RECT, Size(3, 3));

	erode(thresh, thresh, erodeElement2);
	erode(thresh, thresh, erodeElement2);

	dilate(thresh, thresh, dilateElement2);
	dilate(thresh, thresh, dilateElement2);
}


trainedBlocks* trackFilteredObject(Mat threshold, Mat HSV, Mat &cameraFeed, BlockDetector* detector, trainedBlocks* trainedBlocks)
{
	trainedBlock* aBlock = new trainedBlock();
	Mat temp, canny_out;
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<int> alive;

	detector->findCorners(cameraFeed); 

	//cout << "Alive: ";
	//for (int i = 0; i < alive.size(); i++) cout << alive.at(i) << " ";
	//cout << endl;

	//Canny(temp, canny_out, thresh, thresh * 2, 3);
	Canny(temp, canny_out, 0, 50, 5);

	//find contours of filtered image using openCV findContours function
	findContours(canny_out, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//fillPoly(cameraFeed, contours, Scalar(255, 255, 255));

	/*
	for (size_t i = 0; i < contours.size(); i++)
	{
		contours.at(i) = contoursConvexHull(contours.at(i));
	}*/

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

				Moments moment = moments((cv::Mat)contours[index]);
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
						aBlock = trainedBlocks->update(detector, X, Y, contours, index);

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

	if(!objectFound) putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);

	return trainedBlocks; 
}



int main(int argc, char* argv[])
{

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

	cv::Scalar HSVminPlus = cv::Scalar(0, 0, 56); //black
	cv::Scalar HSVmaxPlus = cv::Scalar(256, 256, 256); //black

	//orange color is hardcoded in the Block structure

	//Matrix to store each frame of the webcam feed
	Mat distanceCoefficients, HSV, gray, board;
	Mat threshold_block, threshold_plus, threshold_board, adaptive_threshold_block;
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

	vector<trainedBlock> trackedBlocks;
	vector<vector<Point2f>> markerCorners;
	vector<int> IDs;

	BlockDetector* detector = new BlockDetector();
	//BlockParser parser;
	Board plate;
	trainedBlocks* trainBlocks = new trainedBlocks();

	if (calibrationMode)
	{
		//create slider bars for HSV filtering
		createTrackbars();
	}

	//video capture object to acquire webcam feed

	//VideoCapture capture(1);

	//open capture object at location zero (default location for webcam)
	//capture.open(1);
	//set height and width of capture frame
	//if (!capture.isOpened()) return 0;

	//capture.set(CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	//capture.set(CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);


	//cameraSettings(capture);

	//findMarkers.loadCameraCalibration("CalibrationBlaster", cameraMatrix, distanceCoefficients);
	//detector->loadCameraCalibration("CalibrationIntel", cameraMatrix, distanceCoefficients);
	Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);


	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while (true) {

		Mat cameraFeed, bgr;
		auto rgbStream = selection.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
		auto height = rgbStream.height();
		auto width = rgbStream.width();
		//	cout << height << width << endl;
		rs2::frameset frames = pipe.wait_for_frames();

		if (frames.size() > 0) 
		{
			rs2::frame rgbFrame = frames.first(RS2_STREAM_COLOR);

			bgr = Mat(height, width, CV_8UC3, (void*)rgbFrame.get_data());
			//Mat cameraFeed;
			cvtColor(bgr, cameraFeed, COLOR_BGR2RGB);
			//cvtColor(bgr, src, COLOR_BGR2RGB);
			//imshow("Test", cameraFeed);
			if (waitKey(1000 / 60) >= 0) break;
		}


		//store image to matrix
		//capture.read(cameraFeed);

		//set ROI

		//Rect ROI = Rect(120, 70, 365, 355);
		//Mat image_roi = cameraFeed(ROI);
		//cameraFeed = image_roi;
		//imshow("ROI", image_roi);

		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
		//cvtColor(src, hsv, COLOR_BGR2HSV);
		//convert frame from BGR to GRAY colorspace
		cvtColor(cameraFeed, gray, COLOR_BGR2GRAY);


		if (calibrationMode)
		{
			//if in calibration mode, we track objects based on the HSV slider values.
			//cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold_block);
			inRange(HSV, HSVminPlus, HSVmaxPlus, threshold_plus);
			inRange(HSV, HSVminBoard, HSVmaxBoard, threshold_board); // board

			morphOps(threshold_block);
			morphOps(threshold_plus);
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
			//morphOps(threshold_plus);
			morphOps(threshold_board);

			plate.identifyBoard(threshold_board);

			trainBlocks = trackFilteredObject(threshold_block, HSV, cameraFeed, detector, trainBlocks);
			drawObject(trainBlocks, cameraFeed);

		}

		//show frames 
		//imshow(windowName2,threshold_block);
		//imshow("Plus threshold", threshold_plus);
		//imshow("Board threshold", threshold_board);

		//Mouse params for callback
		MouseParams* mp = new MouseParams();
		mp->blocks = &trainBlocks->tBlocks;
		mp->br = plate.getBottom_right_corner();
		mp->thresh_cycles = threshold_plus; 

		setMouseCallback(windowName, CallBackBlocks, static_cast<void*>(mp));

		imshow(windowName, cameraFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}


