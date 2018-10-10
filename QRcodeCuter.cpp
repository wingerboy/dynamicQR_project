
#include "QRcodeCuter.h"
using namespace std;
using namespace cv;

QRcodeCuter::QRcodeCuter(void)
{

}


QRcodeCuter::~QRcodeCuter(void)
{
}

void QRcodeCuter::QRcode_separate(cv::Mat &src,int thresh_value){
	this->src = src;
	Mat binaryImg,grayImg;
	cvtColor(src,grayImg, CV_BGR2GRAY);
    difference_L8(grayImg,binaryImg,thresh_value);//八步长差分运算，并转化为二进制图像binaryImg
//imshow("binaryImg", binaryImg);//显示差分运算并二值化后的图像
    dilate(binaryImg,binaryImg,Mat(3,1,CV_8U,cvScalar(1)),Point(0,0),1);//膨胀后的图像
//imshow("src3",binaryImg);
	boxes.clear();
	rects.clear();
    Mat_EdgeDetect(binaryImg,10); //得到边界信息，但未画框
//imshow("src3",binaryImg);
	for(int i=0,n=boxes.size();i<n;i++){
		Fe_box2rect(boxes[i]);		
	}
	QRcode_areacut(grayImg);
//imshow("src3",grayImg);
}

void QRcodeCuter::difference_L8(Mat &src,Mat &dest,int thresh_value){
	Mat &binaryImg = dest;
    threshold(src, binaryImg,thresh_value,255,0);//二值化

	int height,width;
	float temp_val,max_val=0;
	height = src.rows;
	width = src.cols;
	Mat tempImg = Mat_<float>(src);
	for(int i=0;i<height;i++){
		for(int j=0;j<width-8;j++){
			temp_val = tempImg.at<float>(i,j) - tempImg.at<float>(i,j+8);
			temp_val = temp_val>=0?temp_val:-temp_val;
            max_val = max_val>temp_val?max_val:temp_val;//算出tempImg.at<float>(i,j) - tempImg.at<float>(i,j+8)的最大值
            tempImg.at<float>(i,j) = temp_val;    //差分运算
		}
	}
	uchar pox_val;
    uchar thresh_value2 = 200;   //差分运算部分的二值化的阈值thresh_value2

	for(int i=0;i<height;i++){
		for(int j=8;j<width;j++){
			pox_val = (uchar)(220 - tempImg.at<float>(i,j-8)/max_val*220);
			pox_val = pox_val>thresh_value2?255:0;
			binaryImg.at<uchar>(i,j) = binaryImg.at<uchar>(i,j)>pox_val?pox_val:binaryImg.at<uchar>(i,j);
		}
	}

}

int QRcodeCuter::Mat_EdgeDetect(Mat &srcImg,int maxnum){
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat binaryImg ;
	srcImg.copyTo(binaryImg);
	findContours( binaryImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	int contourSize = contours.size();
	int count = 0;
	for(int i = 0; i < contourSize; i++) {
		if(hierarchy[i][3] != -1){
			double seqLength = arcLength( contours[i], true );

			Rect rect = boundingRect(contours[i]);
			RotatedRect box = minAreaRect(contours[i]);
			//长宽差
			//double WHRatio = abs(box.size.width - box.size.height);
			double WHRatio = box.size.width/(box.size.height + 1);
			//矩形周长
			double BoxLength = (box.size.width + box.size.height)*2;
			//周长差
			double Length_deviation = abs(seqLength - BoxLength);

			
			//序列面积
			double area = contourArea(contours[i]);
			//矩形面积
			double BoxArea = box.size.area();
			//面积差
			double Area_deviation = abs(BoxArea - area);
			
            /*
			if(seqLength>100){
				cout<<endl;
				cout<<"WHRatio: "<<WHRatio<<endl;
				cout<<"seqLength: "<<seqLength<<endl;
				cout<<"box.size.width: "<<box.size.width<<endl;
				cout<<"box.size.height: "<<box.size.height<<endl;
			}
			*/
			//if ( seqLength > 400 && WHRatio<3) {
            if ( seqLength > 700 &&WHRatio <3 && WHRatio >0.5 && Length_deviation <500) {
				count++;
                /*
				cout<<endl;
				cout<<"WHRatio: "<<WHRatio<<endl;
				cout<<"Length_deviation: "<<Length_deviation<<endl;
				cout<<"seqLength: "<<seqLength<<endl;
				cout<<"Area_deviation: "<<Area_deviation<<endl;
                */
				boxes.push_back(rect);
			}
			if(count > maxnum){
				cout<<"system : above the maxnum !!!"<<endl;
                break;
			}
		}
	}
	return count;
}
/*
int QRcodeCuter::Fe_EdgeDetect(Mat &InputImg,int mode,int maxn_box)
{
	boxes.clear();
	rects.clear();
	//用于保存检测出的有效地积木边框的个数
	int BoxRectCount=0;
	//用于保存每个搜索到的轮廓的序列
	CvSeq *Seq = NULL;	
	//用于保存所有边缘序列的内存块
	CvMemStorage *EdgeStorage = cvCreateMemStorage();
	int EdgeCount;
	//调用cvFindContours函数，在二值化处理后的图像中捕捉边缘序列，序列的个数保存于EdgeCount
	EdgeCount = cvFindContours(&IplImage(InputImg), EdgeStorage , &Seq , sizeof(CvContour) , mode);
	cout<<"EdgeCount ："<<EdgeCount<<endl;
	//循环遍历每一个捕捉到的边缘序列
	for ( ; Seq != NULL ; Seq = Seq->h_next )
	{
		//取某一序列的最小外接矩形
		CvBox2D Box = cvMinAreaRect2( Seq , NULL);


		//计算该序列的周长
		double BoxLength = cvArcLength( Seq );
		
		//计算该序列的面积
		double BoxArea = fabs( cvContourArea(Seq) );
		
		//取最小外接矩形的角度
		double BoxAngle = Box.angle;

		//计算序列的周长平方与面积比，用以滤去细长的不合理边缘序列
		double Length2AreaRate = BoxLength * BoxLength  / BoxArea;

		//根据以下条件，确保边缘序列属于正方形的积木边缘
		if ( (BoxLength >= 400)&&(abs(Box.size.height-Box.size.width)<50) ) 
		//if ( (BoxLength >= 400)) 
		{
			//保存符合积木边缘条件的边缘序列信息
			//BoxRect = cvBoundingRect( Seq , 0 );

			//使用绿色的矩形边框在原图中标识出积木边缘的位置
 			//cvRectangle( OriImg , cvPoint( BoxRect.x , BoxRect.y ) , cvPoint( BoxRect.x + BoxRect.width , BoxRect.y + BoxRect.height ) , CV_RGB( 0 , 255 , 0 ) , 2 );

			//将每一个有效的积木边框序列起始点坐标保存
			//this->BoxRectTable[ this->BoxRectCount ] = BoxRect;
			boxes.push_back(Box);

			//积木个数计数累加
			++ BoxRectCount;
		}
		if (BoxRectCount > maxn_box)
			break;
	}
	cvReleaseMemStorage(&EdgeStorage);
	return BoxRectCount;
}
*/
void QRcodeCuter::drawQRareas(CvScalar color){
	Point pt[4];
    //cout<<"boxes.size()"<<boxes.size()<<endl;
	for(int i = 0,n = boxes.size();i<n;i++){	 
		pt[0].x = boxes[i].x;
		pt[0].y = boxes[i].y;
		pt[1].x = boxes[i].x+boxes[i].width;
		pt[1].y = boxes[i].y;
		pt[2].x = boxes[i].x+boxes[i].width;
		pt[2].y = boxes[i].y+boxes[i].height;
		pt[3].x = boxes[i].x;
		pt[3].y = boxes[i].y+boxes[i].height;

        line( src, pt[0], pt[1],color, 2, 8, 0 );
        line( src, pt[1], pt[2],color, 2, 8, 0 );
        line( src, pt[2], pt[3],color, 2, 8, 0 );
        line( src, pt[3], pt[0],color, 2, 8, 0 );//画红线
	}
}

void QRcodeCuter::QRcode_areacut(Mat &grayImg){
	Mat temp;
	matrixs.clear();
	for(int i=0,n = rects.size();i<n;i++){
		grayImg(rects[i]).copyTo(temp); 
		matrixs.push_back(temp);
//imshow("temp", temp);显示剪切后的图像
	}
}

vector<cv::Mat>& QRcodeCuter::getQRcode(){
	return matrixs;
}

void QRcodeCuter::Fe_box2rect(Rect &aBox)
{
	CvRect aRect;

	/*
	double angle = aBox.angle*CV_PI/180;
    float a = (float)fabs(cos(angle)*0.5f); 
    float b = (float)fabs(sin(angle)*0.5f); 
    aRect.x = (int)(aBox.center.x - b*aBox.size.height - a*aBox.size.width-6); 
	aRect.y = (int)(aBox.center.y - a*aBox.size.height - b*aBox.size.width-6);
	int temp_with = (int)((b*aBox.size.height + a*aBox.size.width)*2+12);
	int temp_height = (int)((a*aBox.size.height + b*aBox.size.width)*2+12);
	*/
	aRect.x = aBox.x;
	aRect.y = aBox.y;
	int temp_with = aBox.width;
	int temp_height = aBox.height;
	int temp_with_dft = getOptimalDFTSize(temp_with);
	int temp_height_dft = getOptimalDFTSize(temp_height);
	int x_offset = (temp_with_dft - temp_with)/2;
	int y_offset = (temp_height_dft - temp_height)/2;
	if(aRect.x>x_offset && aRect.y>y_offset && (aRect.x+temp_with_dft+1)<src.cols && (aRect.y+temp_height_dft+1)< src.rows){
		aRect.x = aRect.x - x_offset;
		aRect.y = aRect.y - y_offset;
		aRect.width = temp_with_dft;
		aRect.height = temp_height_dft;
	}
	else{
		aRect.width = temp_with;
		aRect.height = temp_height;
	}
	rects.push_back(aRect);
}

vector<CvRect>& QRcodeCuter::getRects(){
	return rects;
}
