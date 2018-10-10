#pragma once
#include <opencv2/opencv.hpp>

class Blurlen_detect
{
public:
	Blurlen_detect(void);
	~Blurlen_detect(void);
	//This function is able to get blured len of motion img
	//The 1st parament gray img of uchar type ,which size is n*m
	//The result is blured len of img;
	int get_blurlen(const cv::Mat &);
private:
	//This function is able to accumulate autocorrelation by fourier
	//The 1st parament a matrix of uchar type ,which size is 1*m,it's one row of gray img
	cv::Mat get_rowaucolation(const cv::Mat &);
private:
	int blurlen;
	cv::Mat aucolation_row;
};

