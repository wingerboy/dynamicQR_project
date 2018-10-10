#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class QRcodeCuter
{
public:
	QRcodeCuter(void);
	~QRcodeCuter(void);
	void QRcode_separate(cv::Mat &,int);
	void drawQRareas(CvScalar color);
	std::vector<cv::Mat>& getQRcode();
	std::vector<CvRect>& getRects();
private:
	int Mat_EdgeDetect(cv::Mat &binaryImg,int maxnum);
	void QRcode_areacut(cv::Mat &);
	void Fe_box2rect(cv::Rect &aBox);
	void difference_L8(cv::Mat &src,cv::Mat &dest,int thresh_value);

private:
	std::vector<cv::Mat> matrixs;
	std::vector<cv::Rect> boxes;
	std::vector<CvRect> rects;
	cv::Mat src;

};

