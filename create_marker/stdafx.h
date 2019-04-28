// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\aruco.hpp>
#include <opencv2\core\types.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\calib3d.hpp>


#include <librealsense2/rs.hpp>
#include <omp.h>
#include <execution>

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const double MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;
//block dimension
const int BLOCK_WIDTH = 40;
const int BLOCK_HEIGHT = 40;

// TODO: reference additional headers your program requires here
