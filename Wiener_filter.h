#pragma once
#include <opencv2/opencv.hpp>
#include"basicoperation.h"

class Wiener_filter
{
public:
	Wiener_filter(void);
	~Wiener_filter(void);
	//Use wiener_filter to recovery blured img,it will not change original img ,but create a new datastore;
	//The 1st parament is gray img with uchar type , 1 channel and 2 dimensions
	//The 2nd parament is blured len ,make sure it > 0
	//The 3td parament is SNR ,make sure it >0 as well and <1 of course
	//The result is a matrix with uchar type, 1 channel and 2 dimensions,and maybe it will bigger than original img
	cv::Mat get_recov_mat(const cv::Mat &,int,float);
	//To get the original size of recovery img,and it uses the same datastore of result of function get_recov_mat.
	cv::Mat get_orisize_mat();
    //*功能：  实现维纳滤波
    void wnrFilter1(cv::Mat &input, cv::Mat &kernal, cv::Mat &output);
    //*功能：  实现改进的维纳滤波,采用边界延拓抑制振铃
    void wnrFilter2(cv::Mat &input, cv::Mat &kernal, cv::Mat &output);

private:
	//This function is able to creat psf by blured len,because of the symmetry of psf ,we move half of cycle left ,so that we can get real matrix psf,expecially,if the len is not odd ,we will add one pix to make sure it's symmetrical;
	//e.g.
	//	 cols = 8;
	//   len = 4;
	//   arr = [0.25, 0.25, 0.125, 0, 0, 0, 0.125, 0.25];		//reversed psf
	//	and the use dft to deal with arr and copy some rows to get frequency domain of psf
	//	The 1st parament is rows ,the 2nd is cols, the last is len
	void generate_psf(int,int,int);
	//wiener_filter is a frequency domain matrix, and it is only real domain,so the wiener_filter is a matrix like this:
	//H(u,v)/(H(u,v)^2+snr) accumulate by point.
	void gen_wiener_filter(float);
    cv::Mat edgetaper(cv::Mat &image, cv::Mat &psf);

private:
	cv::Mat psf;
	cv::Mat recov_mat;
	int ori_rows,ori_cols;
};

