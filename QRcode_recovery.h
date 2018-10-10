#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <stack>
#include <math.h>
#include <algorithm>
typedef struct squence_index{
	int index;
	int level;
    //struct squence_index():index(-1),level(-1){};
    //struct squence_index(int input_index,int input_level):index(input_index),level(input_level){};
}Squence_index;

typedef struct locators{
	cv::RotatedRect rect;
	int index;
    //struct locators():rect(),index(-1){};
    //struct locators(cv::RotatedRect input_Rect,int input_index):rect(input_Rect),index(input_index){};
}Locators;

class QRcode_recovery
{
public:
	QRcode_recovery(void);
	~QRcode_recovery(void);
	//This function is able to recovery the QRcode, which have been recoveried by wiener,
	//Features: it will not change the original img
	//The 1st parament is gray img with uchar type ,1 channel and 2 dimensions
	//The result is a Mat ,the size of Mat is equal to the blocks of QRcode ,or 1 pixel is equal to 1 block of QRcode
	bool try_recovery(cv::Mat &);
	//This function is able to return the recoveried Img;
	cv::Mat get_recovImg();
	//This function is able to get the rects of locators in QRcode
	std::vector<cv::Rect> &getRects();
	//This function is able to get the block information of QRcode,include startpoint,the lenth of locators, the size of locator;
	//Features: result is only 7 elements ,which are :
	//[0]startpoint.x [1]startpoint.y [2]QRcode_size [3]blocksize [4]locator.x [5]locator.y [6]the length of two horizontal locators;
	std::vector<float> getQRcodeblockinfo();
	//This function is able to recalculate QRcode information based on input QRcodeType;
	//The 1st parament is QRcodeType;
	void recalculQRcodeinfo(int);
	//This function is able to get the recovered QRcode, and one block equal one pixel
	cv::Mat get_recovQRcode();
	//This function is different from get_recovQRcode,to show the Img of QRcode ,the size of Img must be n times bigger than QR code;
	cv::Mat get_recovQRcodeImg();
private:
	//This function is able to search the locator squences in QRcode ,The locator is at the 6th level in CV_RETR_TREE.
	void DFS_locators(int,int,int);
	void find_locators(int);
	//This function is able to transform edge squence to Rect and RotatedRect
	bool transtoRect();
	//This function is able to calculate affinity matrix to affine src_Img;
	void affine_transform(cv::Point2f *);
	//This function is able to calculate the order of each point;
	void points_sort(cv::Point2f *);
	//This function is able to calculate the QRcode information;
	//The 1st parament is locator points
	//The 2nd parament is the length of two horizontal locators;
	void calculQRcodeinfo(cv::Point2f *,float);
    //This function is able to calculate the rate of each model in QRcode
    //The 1st parament is binary img
    //The 2nd parament is row-start point
    //The 3th parament is row-end point ,which is included
    //The 4th parament is column-start point
    //The 5th parament is column-end point ,which is included
    //The return is rate of the model
    float ratecalculate(cv::Mat &src,int rstart,int rend,int cstart,int cend);
private:
	std::vector<cv::Rect> rects,father_rects;
	std::vector<cv::RotatedRect> boxes,father_boxes;
	cv::Mat src_Img,dest_Img;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::stack<Squence_index> dfstack;
	std::vector<int> locator_index;
	std::vector<float> QRcodeinfo;
	cv::Mat QRcode,QRcodeImg;
	float blockwidth;
	int QRcodeType;
};

