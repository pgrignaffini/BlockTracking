#pragma once
class Board
{
public:
	Board();
	~Board();

	std::string intToString(int number);

	void identifyBoard(cv::Mat threshold_board);
	void printBoard(cv::Mat threshold_board);

	cv::Scalar getHSVmin();
	cv::Scalar getHSVmax();
	cv::Point getBottom_right_corner();
	cv::Point getBottom_left_corner();
	cv::Point getTop_left_corner();
	cv::Point getTop_right_corner();
	std::vector<std::vector<cv::Point>> getBoard_contours();
	std::vector<cv::Vec4i> getBoard_hierarchy();
	cv::Point2f* getRectPoints();

	void setHSVmin(cv::Scalar _hsvmin);
	void setHSVmax(cv::Scalar _hsvmax);
	void setBottom_right_corner(cv::Point br);
	void setBottom_left_corner(cv::Point lr);
	void setTop_left_corner(cv::Point tl);
	void setTop_right_corner(cv::Point tr);
	void setBoard_contours(std::vector<std::vector<cv::Point>> _contours);
	void setBoard_hierarchy(std::vector<cv::Vec4i> _hierarchy);
	void setRectPoints(cv::Point2f* points);

private:

	cv::Scalar HSVmin;
	cv::Scalar HSVmax;
	cv::Point bottom_right_corner;
	cv::Point bottom_left_corner;
	cv::Point top_left_corner;
	cv::Point top_right_corner;
	std::vector<std::vector<cv::Point>> board_contours;
	std::vector<cv::Vec4i> board_hierarchy;
	cv::Point2f* rectpoints;

	std::vector<cv::Point> contoursConvexHull(std::vector<std::vector<cv::Point>> contours);

};

