#include "QRcode_recovery.h"
#include <QDebug>
using namespace std;
using namespace cv;

QRcode_recovery::QRcode_recovery(void)
{
}


QRcode_recovery::~QRcode_recovery(void)
{
}

bool QRcode_recovery::try_recovery(Mat &src){
    src_Img = src;//剪切后红框框中的图像
	rects.clear();
	boxes.clear();
	father_rects.clear();
	father_boxes.clear();
	QRcodeinfo.clear();

	find_locators(6);
	bool flag = transtoRect();
	Point2f points[3];
	if(flag){
		points_sort(points);
		affine_transform(points);
//imshow("fang1",dest_Img);
        //cout<<"try_recovery"<<endl;
		return true;
	}
    find_locators(4);//????判断是否需要进行仿射变换
	flag = transtoRect();
	if(flag){
		points_sort(points);
        affine_transform(points);//仿射变换
//imshow("fang2",dest_Img);
//cout<<"try_recovery"<<endl;
		return true;
	}
	else{
		cout<<"locators are not enough !!!"<<endl;
		return false;
	}
	return true;
}

vector<Rect> & QRcode_recovery::getRects(){
	return rects;
}

void QRcode_recovery::DFS_locators(int index,int level,int targetlevel){
    Squence_index temp;
    temp.index = index;
    temp.level = level;
    Squence_index child_temp;
	dfstack.push(temp);
	int child_index;
	while(!dfstack.empty()){
		temp = dfstack.top();
		dfstack.pop();
		if(temp.level == targetlevel){
            if( targetlevel == 6 || hierarchy[temp.index][2] == -1)
				locator_index.push_back(temp.index);
			continue;
		}
		child_index = hierarchy[temp.index][2];
		if(child_index != -1){
			child_temp.index = child_index;
			child_temp.level = temp.level + 1;
			dfstack.push(child_temp);
			while(hierarchy[child_index][0]!=-1){
				child_index = hierarchy[child_index][0];
				child_temp.index = child_index;
				child_temp.level = temp.level + 1;
				dfstack.push(child_temp);
			}
		}
	}

}

void QRcode_recovery::find_locators(int targetlevel){
	contours.clear();
	hierarchy.clear();
	locator_index.clear();
	cv::Mat binaryImg ;
	threshold(src_Img, binaryImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//imshow("erzhihua",binaryImg);
    dilate(binaryImg,binaryImg,Mat(1,1,CV_8U),Point(-1,-1),1);
    erode(binaryImg,binaryImg,Mat(2,2,CV_8U),Point(-1,-1),1);
//imshow("erzhihua",binaryImg);
    findContours( binaryImg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);//binaryImg必须为二值单通道图像
//imshow("src4",binaryImg);
    int contourSize = contours.size();
	int count = 0;
    DFS_locators(0,1,targetlevel);
	int roots = hierarchy[0][0];
	while(roots != -1){
		DFS_locators(roots,1,targetlevel);
		roots = hierarchy[roots][0];
	}	
}

bool QRcode_recovery::transtoRect(){
	Rect rect,rect_array[3],rect_temp;
	RotatedRect box;
	Locators box_array[3],box_temp;
	vector<int> fatherbox(3);
	int count = 0,squence_index;
    /*
	for(int i = 0;i<3;i++){
		rect_array[i] = Rect();
		box_array[i] = Locators();
	}
    */
    //cout<<"locator_index.size() : "<<locator_index.size()<<endl;
	for(int i = 0,n = locator_index.size();i<n;i++){
		squence_index = locator_index[i];
		rect = boundingRect(contours[squence_index]);
		box = minAreaRect(contours[squence_index]);
		//序列长度
		double seqLength = arcLength( contours[squence_index], true );
        //cout<<endl;
        //cout<<"seqLength : "<<seqLength<<endl;
		//长宽比
		double WHRatio = box.size.width/(box.size.height + 1);
        //cout<<"box.size.width : "<<box.size.width<<endl;
        //cout<<"WHRatio : "<<WHRatio<<endl;
		//矩形周长
		double BoxLength = (box.size.width + box.size.height)*2;
		//周长比
		double LengthRatio = seqLength / (BoxLength+1);
        //cout<<"LengthRatio : "<<LengthRatio<<endl;
		//序列面积
		double area = contourArea(contours[squence_index]);
        //矩形面积
		double BoxArea = box.size.area();
		//面积比
		double AreaRatio = area / (BoxArea +1);
        //cout<<"AreaRatio : "<<AreaRatio<<endl;
		
        if( seqLength > 30 && WHRatio <3 && WHRatio>0.3 &&
            LengthRatio <2 && LengthRatio > 0.4 &&
            AreaRatio < 2 && AreaRatio > 0.4){
		
		//if(1){
			count ++;
			if(box_array[1].rect.size.width < box.size.width ){
                Locators box_temp;
                box_temp.rect = box;
                box_temp.index = i;
                box_array[1] = box_temp;
				rect_array[1] = rect;
				if(box_array[1].rect.size.width > box_array[0].rect.size.width){
					box_temp = box_array[1];
					box_array[1] = box_array[0];
					box_array[0] = box_temp;

					rect_temp = rect_array[1];
					rect_array[1] = rect_array[0];
					rect_array[0] = rect_temp;
				}
				if(box_array[1].rect.size.width > box_array[2].rect.size.width){
					box_temp = box_array[1];
					box_array[1] = box_array[2];
					box_array[2] = box_temp;

					rect_temp = rect_array[1];
					rect_array[1] = rect_array[2];
					rect_array[2] = rect_temp;
				}
			}
		}
	}
	if(count < 3) return false;

	else{
		vector<float> lenth_array,father_length;
		int box_index,contours_index,father_index;
		for(int i = 0;i<3;i++){
			rects.push_back(rect_array[i]);
			boxes.push_back(box_array[i].rect);
			lenth_array.push_back(box_array[i].rect.size.width);
			lenth_array.push_back(box_array[i].rect.size.height);

			box_index = box_array[i].index;
			contours_index = locator_index[box_index];
			father_index = hierarchy[contours_index][3];
			rect = boundingRect(contours[father_index]);
			box = minAreaRect(contours[father_index]);
			father_rects.push_back(rect);
			father_boxes.push_back(box);
			father_length.push_back(box.size.width);
			father_length.push_back(box.size.height);

		}
		sort(lenth_array.begin(),lenth_array.end());
		sort(father_length.begin(),father_length.end());

		//blockwidth = (lenth_array[2] + lenth_array[3])/2;
		blockwidth = (lenth_array[0]+father_length[0])/8;
	}
	return true;
}

void QRcode_recovery::affine_transform(cv::Point2f *points){
	Point2f dest_points[3],src_points[3];

	for(int i = 0;i < 3;i++){
		src_points[i] = points[i];
	}
	float delta_x = points[0].x - points[1].x;
	float delta_y = points[0].y - points[1].y;
	float length = sqrt(delta_x*delta_x +delta_y*delta_y);

	dest_points[0] = points[0];
	dest_points[1].x = points[0].x + length;
	dest_points[1].y = points[0].y;
	dest_points[2].x = points[0].x;
	dest_points[2].y = points[0].y + length;
	Mat warp_mat;
	warp_mat = getAffineTransform( src_points, dest_points );
    warpAffine( src_Img, dest_Img, warp_mat,cv::Size(src_Img.cols*2,src_Img.rows*2));//仿射变换
	calculQRcodeinfo(points,length);
}

void QRcode_recovery::points_sort(Point2f *points){
	Point2f points_temp[3],vector_array[3];
	float varA_x,varA_y,varB_x,varB_y;
	float cartesian_product,cross_product,min_carproduct = FLT_MAX;
	int target_index;
	for(int i = 0;i < 3;i++){
		points_temp[i] = boxes[i].center;
	}
	for(int i = 0;i < 3;i++){
		varA_x = points_temp[(i+1) % 3].x - points_temp[i].x;
		varA_y = points_temp[(i+1) % 3].y - points_temp[i].y;
		varB_x = points_temp[(i+2) % 3].x - points_temp[i].x;
		varB_y = points_temp[(i+2) % 3].y - points_temp[i].y;
		cartesian_product = varA_x*varB_x + varA_y*varB_y;
		if(min_carproduct > cartesian_product){
			min_carproduct = cartesian_product;
			target_index = i;
		}
	}
	points[0] = points_temp[target_index];
	varA_x = points_temp[(target_index+1) % 3].x - points_temp[target_index].x;
	varA_y = points_temp[(target_index+1) % 3].y - points_temp[target_index].y;
	varB_x = points_temp[(target_index+2) % 3].x - points_temp[target_index].x;
	varB_y = points_temp[(target_index+2) % 3].y - points_temp[target_index].y;
	cross_product = varA_x*varB_y - varA_y*varB_x;
	if(cross_product > 0){
		points[1] = points_temp[(target_index+1) % 3];
		points[2] = points_temp[(target_index+2) % 3];
	}
	else{
		points[1] = points_temp[(target_index+2) % 3];
		points[2] = points_temp[(target_index+1) % 3];		
	}
}

vector<float> QRcode_recovery::getQRcodeblockinfo(){
	return QRcodeinfo;
}

Mat QRcode_recovery::get_recovImg(){
    return dest_Img;
}

void QRcode_recovery::calculQRcodeinfo(cv::Point2f *points,float length){
	float start_temp_x = points[0].x;
	float start_temp_y = points[0].y;
	int QRsize_temp = (int)(length/blockwidth+0.5);
    int QRlevel = (QRsize_temp - 12)/4;//debug输出的QRsize_temp=18
//qDebug()<<QRsize_temp;
	QRcodeType = 21 + QRlevel * 4;
	float QRblocksize = length/(QRcodeType-7);
//qDebug()<<QRblocksize;
	QRcodeinfo.push_back(points[0].x - 3.5* QRblocksize + 0.5);
	QRcodeinfo.push_back(points[0].y - 3.5* QRblocksize + 0.5);
	QRcodeinfo.push_back(QRblocksize*QRcodeType);
	QRcodeinfo.push_back(QRblocksize);
	QRcodeinfo.push_back(start_temp_x);
	QRcodeinfo.push_back(start_temp_y);
	QRcodeinfo.push_back(length);

}

void QRcode_recovery::recalculQRcodeinfo(int QRcodeType){
	float QRblocksize = QRcodeinfo[6]/(QRcodeType-7);
	QRcodeinfo[0] = QRcodeinfo[4] - 3.5*QRblocksize + 0.5;
	QRcodeinfo[1] = QRcodeinfo[5] - 3.5*QRblocksize + 0.5;
	QRcodeinfo[2] = QRblocksize * QRcodeType;
	QRcodeinfo[3] = QRblocksize;
}

cv::Mat QRcode_recovery::get_recovQRcode(){
	Mat binaryImg ;
	Mat QRcodeArea(dest_Img, Rect((int)QRcodeinfo[0],(int)QRcodeinfo[1],(int)QRcodeinfo[2]+1,(int)QRcodeinfo[2]+1));
	threshold(QRcodeArea, binaryImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//imwrite("C:/Users/Administrator/Desktop/recoverImg.jpg", QRcodeArea);/*这个图像被宽度被压缩了大概1/3，但是位深度为24*/
//imshow("src0",dest_Img);//这个图像是红色检测框切割后的图像
//imshow("src4",binaryImg);
    Point pt[4];
    for(int i = 0;i < 26;i++){
        pt[0].x =(int)(i*QRcodeinfo[3]+0.5);
        pt[0].y = 0;
        pt[1].x =(int)(i*QRcodeinfo[3]+0.5);
        pt[1].y = QRcodeinfo[2];
    line( binaryImg, pt[0], pt[1],255, 1, 8, 0 );
    }
//imshow("src4",binaryImg);
    for(int i = 0;i < 26;i++){
        pt[0].x = 0 ;
        pt[0].y = 0 + (int)(i*QRcodeinfo[3]+0.5);
        pt[1].x = 0 + QRcodeinfo[2];
        pt[1].y = 0 + (int)(i*QRcodeinfo[3]+0.5);
    line( binaryImg, pt[0], pt[1],255, 1, 8, 0 );
    }
//imshow("src4",binaryImg);
//imwrite("C:/Users/Administrator/Desktop/binaryImg.jpg", binaryImg);
	int QRcodeType =(int)( QRcodeinfo[2] / QRcodeinfo[3] + 0.5);
//qDebug()<<QRcodeType;
    QRcode = Mat::zeros(cvSize(QRcodeType,QRcodeType), binaryImg.type());//CV_8UC1
    QRcodeImg = Mat::zeros(cvSize(QRcodeType*20,QRcodeType*20), binaryImg.type());//CV_8UC1
    Mat QRcode2 = Mat::zeros(cvSize(QRcodeType,QRcodeType), CV_32FC1);
//imwrite("C:/Users/Administrator/Desktop/QRcodeImg.jpg", QRcodeImg);
	for(int i = 0;i < QRcodeType;i++){
		for(int j = 0;j< QRcodeType;j++){
			float sum = 0;
            int size = 0;
            /*
			for(int start_y = (int)(i*QRcodeinfo[3]+QRcodeinfo[0]), n = (int)((i+1)*QRcodeinfo[3]+0.5+QRcodeinfo[0]);start_y < n;start_y++){
				for(int start_x = (int)(j*QRcodeinfo[3]+QRcodeinfo[1]), m = (int)((j+1)*QRcodeinfo[3]+0.5+QRcodeinfo[1]); start_x < m;start_x++){
					sum += (float)binaryImg.at<uchar>(start_x,start_y);
					size ++;
				}
			}
            */

            int r_start = (int)(i*QRcodeinfo[3]+0.5);
            int r_end = (int)((i+1)*QRcodeinfo[3]+0.5)-1;
            int c_start = (int)(j*QRcodeinfo[3] + 0.5);
            int c_end = (int)((j+1)*QRcodeinfo[3]+0.5)-1;
            float rate = ratecalculate(binaryImg,r_start,r_end,c_start,c_end);
//imshow("src4",binaryImg);

            /*
			for(int start_y = (int)(i*QRcodeinfo[3]+0.5), n = (int)((i+1)*QRcodeinfo[3]+0.5);start_y < n;start_y++){
				for(int start_x = (int)(j*QRcodeinfo[3] + 0.5), m = (int)((j+1)*QRcodeinfo[3]+0.5); start_x < m;start_x++){
					sum += (float)binaryImg.at<uchar>(start_y,start_x);
					size ++;
				}
			}

            float rate = size > 0? sum / (size*255) : 0;
            */
            //cout<<"\t"<<rate;


            QRcode.at<uchar>(i,j) = rate > 0.6 ?255:0;
            bool wbflag = rate > 0.6 ?true : false;
			for(int Img_y = i*20, Img_n = (i+1)*20;Img_y < Img_n;Img_y++){
				for(int Img_x = j*20, Img_m =(j+1)*20;Img_x < Img_m;Img_x++){
					QRcodeImg.at<uchar>(Img_y,Img_x) =wbflag?255:0;
					//QRcodeImg.at<uchar>(Img_x,Img_y) = (uchar)rate;
				}
			}


            //QRcode2.at<float>(i,j) = rate;
		}
        //cout<<endl;
	}
    /*
    for(int i = 0;i < QRcodeType;i++){
        for(int j = 0;j< QRcodeType;j++){
            QRcode.at<uchar>(i,j) = (uchar)(QRcode2.at<float>(i,j)*255);
        }
    }
    threshold(QRcode, QRcode, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    for(int i = 0;i < QRcodeType;i++){
        for(int j = 0;j< QRcodeType;j++){
            uchar wbflag = QRcode.at<uchar>(i,j);
            for(int Img_y = i*20, Img_n = (i+1)*20;Img_y < Img_n;Img   for(int Img_x = j*20, Img_m =(j+1)*20;Img_x < Img_m;Img_x++){
                    QRcodeImg.at<uchar>(Img_y,Img_x) =wbflag;
                    //QRcodeImg.at<uchar>(Img_x,Img_y) = (uchar)rate;
                }
            }
        }
    }
    */





//imshow("src5",binaryImg);
//imshow("src6",QRcodeImg);
//imshow("dest_Img",dest_Img);
//imwrite("C:/Users/Administrator/Desktop/QRcodeImg1.jpg", QRcodeImg);
    return QRcode;
}

cv::Mat QRcode_recovery::get_recovQRcodeImg(){
//imshow("QRcodeImg",QRcodeImg);
    return QRcodeImg;
}

float QRcode_recovery::ratecalculate(cv::Mat &src,int rstart,int rend,int cstart,int cend){
    float sum = 0;
    float weight = 1;
    float max_val = 255;
    float all = 0;
    float rate = 0;
    int r_start = rstart;
    int c_start = cstart;
    int rows = rend;
    int cols = cend;
    while((r_start<rows)&&(c_start<cols)){
        for(int i = c_start;i<=cols;i++){
            sum += src.at<uchar>(r_start,i)*weight;
            sum += src.at<uchar>(rows,i)*weight;
        }
        all += (cols-c_start+1)*2*weight*max_val;
        if(rows-r_start > 1){
            for(int i = r_start+1;i<=rows-1;i++){
                sum += src.at<uchar>(i,c_start)*weight;
                sum += src.at<uchar>(i,cols)*weight;
            }
            all += (rows-r_start-1)*2*weight*max_val;
        }
        c_start++;
        r_start++;
        rows--;
        cols--;
        weight = weight*2;
    }
    if(rows == r_start){
        for(int i=c_start;i<=cols;i++){
            sum += src.at<uchar>(r_start,i)*weight;
        }
        all += (cols-c_start+1)*weight*max_val;
    }
    else if(cols == c_start){
        for(int i=r_start;i<=rows;i++){
            sum += src.at<uchar>(i,c_start)*weight;
        }
        all += (rows-r_start+1)*weight*max_val;
    }
    if(all > 0.00001)
        rate = sum / all;
    else
        rate = 1;
    return rate;
}
