#include "Blurlen_detect.h"
using namespace cv;

Blurlen_detect::Blurlen_detect(void)
{
}


Blurlen_detect::~Blurlen_detect(void)
{
}

int Blurlen_detect::get_blurlen(const cv::Mat &src){
	int row_start = src.rows/2-8;
	int aucolation_cols = getOptimalDFTSize(2*(src.cols-1));
	aucolation_row = Mat::zeros(1,aucolation_cols,CV_32FC1);
	for(int i=0;i<5;i++){
		aucolation_row += get_rowaucolation(src.row(row_start+i*4));
	}
	float min_aucolation = 0;
	for(int i = 0,n = aucolation_cols/2;i<n;i++){
		if(aucolation_row.at<float>(0,i)<min_aucolation){
			min_aucolation = aucolation_row.at<float>(0,i);
			blurlen = i+1;
		}
	}
	return blurlen;
}

Mat Blurlen_detect::get_rowaucolation(const cv::Mat &src){
	int aucolation_cols = getOptimalDFTSize(2*(src.cols-1));
	Mat diff_mat = Mat::zeros(1,aucolation_cols,CV_32SC1);
	for(int i=0;i<src.cols-1;i++){
		diff_mat.at<int>(0,i) = src.at<uchar>(0,i+1) - src.at<uchar>(0,i);
	}
	Mat planes[] = {Mat_<float>(diff_mat), Mat::zeros(diff_mat.size(), CV_32F)}; 
	Mat  completeI;  
    merge(planes,2,completeI);
	dft(completeI,completeI);
	split(completeI,planes); 
	float temp1,temp2;
	for(int i=0,n = diff_mat.cols;i<n;i++){
		temp1 = planes[0].at<float>(0,i);
		temp2 = planes[1].at<float>(0,i);
		planes[0].at<float>(0,i) = temp1*temp1+temp2*temp2;
	}
	planes[1] = Mat::zeros(diff_mat.size(), CV_32F);
	merge(planes,2,completeI);
	dft(completeI,completeI,DFT_INVERSE + DFT_SCALE);
	split(completeI,planes); 
	return planes[0];
}
