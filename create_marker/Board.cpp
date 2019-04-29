#pragma once
#include "stdafx.h"
#include "Board.h"

Board::Board()
{
	setHSVmin(cv::Scalar(50, 0, 100)); //gray
	setHSVmax(cv::Scalar(256, 256, 256)); //gray
	bottom_right_corner = cv::Point();
	bottom_left_corner = cv::Point();
	top_left_corner = cv::Point();
	top_right_corner = cv::Point();
	bottom_line = cv::Rect();
	board_contours = std::vector<std::vector<cv::Point>>();
	board_hierarchy = std::vector<cv::Vec4i>();
	rectpoints = new cv::Point2f();
}


Board::~Board()
{
}

struct yGreater {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
} yGreater;

struct xGreater {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
}xGreater;

std::string Board::intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}


std::vector<cv::Point> Board::contoursConvexHull(std::vector<std::vector<cv::Point>> contours)
{
	std::vector<cv::Point> result;
	std::vector<cv::Point> pts;
	for (size_t i = 0; i < contours.size(); i++)
		for (size_t j = 0; j < contours[i].size(); j++)
			pts.push_back(contours[i][j]);
	cv::convexHull(pts, result);
	return result;
}

void Board::printBoard(cv::Mat threshold_board)
{
	cv::Mat board_canny_out;
	Canny(threshold_board, board_canny_out, 0, 50, 5);
	//cv::Mat drawing_board = cv::Mat::zeros(board_canny_out.size(), CV_8UC3);
	cv::Mat drawing_board2 = cv::Mat::zeros(board_canny_out.size(), CV_8UC3);

	cv::Point br = getBottom_right_corner();
	cv::Point tr = getTop_right_corner();
	cv::Point bl = getBottom_left_corner();
	cv::Point tl = getTop_left_corner();

	for (int j = 0; j < 4; j++)
	{
		line(drawing_board2, *(getRectPoints() + j), *(getRectPoints() + ((j + 1) % 4)), cv::Scalar(0, 255, 255), 1, 8);
	}

	///Display Board rectangle
	circle(drawing_board2, br, 10, cv::Scalar(255, 0, 0));
	cv::putText(drawing_board2, "BR", br, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
	cv::putText(drawing_board2, intToString(br.x) + "," + intToString(br.y), cv::Point(br.x - 30, br.y + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
	circle(drawing_board2, tr, 10, cv::Scalar(255, 0, 0));
	cv::putText(drawing_board2, "TR", tr, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
	cv::putText(drawing_board2, intToString(tr.x) + "," + intToString(tr.y), cv::Point(tr.x - 30, tr.y + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
	circle(drawing_board2, bl, 10, cv::Scalar(255, 0, 0));
	cv::putText(drawing_board2, "BL", bl, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
	cv::putText(drawing_board2, intToString(bl.x) + "," + intToString(bl.y), cv::Point(bl.x - 30, bl.y + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
	circle(drawing_board2, tl, 10, cv::Scalar(255, 0, 0));
	cv::putText(drawing_board2, "TL", tl, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255));
	cv::putText(drawing_board2, intToString(tl.x) + "," + intToString(tl.y), cv::Point(tl.x - 30, tl.y + 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));

	cv::namedWindow("Board rect", cv::WINDOW_AUTOSIZE);
	imshow("Board rect", drawing_board2);

	///Display ContoursHulls
	//polylines(drawing_board, ConvexHullPoints, true, cv::Scalar(0, 0, 255), 2);
	//imshow("ContoursHull", drawing_board);
}

void Board::identifyBoard(cv::Mat threshold_board)
{
	cv::Mat board_canny_out;
	Canny(threshold_board, board_canny_out, 0, 50, 5);
	findContours(threshold_board, board_contours, board_hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	setBoard_contours(board_contours);
	setBoard_hierarchy(board_hierarchy);

	if (!board_contours.empty())
	{
		cv::Point2f topleft, topright, bottomleft, bottomright;
		cv::Mat drawing_board = cv::Mat::zeros(board_canny_out.size(), CV_8UC3);
		cv::Mat drawing_board2 = cv::Mat::zeros(board_canny_out.size(), CV_8UC3);

		for (int i = 0; i < board_contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(255, 255, 255);
			drawContours(drawing_board, board_contours, i, color, 2);
		}

		std::vector<cv::Point> ConvexHullPoints = contoursConvexHull(board_contours);
		cv::RotatedRect board_rect = cv::minAreaRect(ConvexHullPoints);
		cv::Point2f rectpoint[4];

		board_rect.points(rectpoint);
		setRectPoints(rectpoint);

		std::vector<cv::Point2f> rectpoints_vec(rectpoint, rectpoint + sizeof rectpoint / sizeof rectpoint[0]);
		
		std::sort(rectpoints_vec.begin(), rectpoints_vec.end(), xGreater);

		if (rectpoints_vec.at(0).y > rectpoints_vec.at(1).y)
		{
			bottomleft = rectpoints_vec.at(0);
			topleft = rectpoints_vec.at(1);
		}

		else
		{
			bottomleft = rectpoints_vec.at(1);
			topleft = rectpoints_vec.at(0);
		}

		if (rectpoints_vec.at(2).y > rectpoints_vec.at(3).y)
		{
			bottomright = rectpoints_vec.at(2);
			topright = rectpoints_vec.at(3);
		}

		else
		{
			bottomright = rectpoints_vec.at(3);
			topright = rectpoints_vec.at(2);
		}

		setBottom_left_corner(bottomleft);
		setBottom_right_corner(bottomright);
		setTop_left_corner(topleft);
		setTop_right_corner(topright);

		cv::Point bline_topleft = bottomleft;
		bline_topleft.y -= 70; //last line height

		cv::Rect bline = cv::Rect(bline_topleft, bottomright);
		setBottom_line(bline);
		///display bottom line
		//cv::rectangle(window, bline, cv::Scalar(0, 0, 255), 4);
		///display window with board
		printBoard(threshold_board);
		return;

	}
}



cv::Scalar Board::getHSVmin()
{
	return Board::HSVmin;
}

cv::Scalar Board::getHSVmax()
{
	return Board::HSVmax;
}

cv::Point Board::getBottom_right_corner()
{
	return Board::bottom_right_corner;
}

cv::Point Board::getBottom_left_corner()
{
	return Board::bottom_left_corner;
}

cv::Point Board::getTop_left_corner()
{
	return Board::top_left_corner;
}

cv::Point Board::getTop_right_corner()
{
	return Board::top_right_corner;
}

std::vector<std::vector<cv::Point>> Board::getBoard_contours()
{
	return Board::board_contours;
}

std::vector<cv::Vec4i> Board::getBoard_hierarchy()
{
	return Board::board_hierarchy;
}

cv::Point2f * Board::getRectPoints()
{
	return Board::rectpoints;
}

cv::Rect Board::getBottomLine()
{
	return Board::bottom_line;
}

void Board::setHSVmin(cv::Scalar _hsvmin)
{
	Board::HSVmin = _hsvmin;
}

void Board::setHSVmax(cv::Scalar _hsvmax)
{
	Board::HSVmax = _hsvmax;
}

void Board::setBottom_right_corner(cv::Point br)
{
	Board::bottom_right_corner = br;
}

void Board::setBottom_left_corner(cv::Point lr)
{
	Board::bottom_left_corner = lr;
}

void Board::setBottom_line(cv::Rect bline)
{
	Board::bottom_line = bline;
}

void Board::setTop_left_corner(cv::Point tl)
{
	Board::top_left_corner = tl;
}

void Board::setTop_right_corner(cv::Point tr)
{
	Board::top_right_corner = tr;
}

void Board::setBoard_contours(std::vector<std::vector<cv::Point>> _contours)
{
	Board::board_contours = _contours;
}

void Board::setBoard_hierarchy(std::vector<cv::Vec4i> _hierarchy)
{
	Board::board_hierarchy = _hierarchy;
}

void Board::setRectPoints(cv::Point2f * points)
{
	Board::rectpoints = points;
}

