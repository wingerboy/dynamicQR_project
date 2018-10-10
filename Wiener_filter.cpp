#include "Wiener_filter.h"

using namespace cv;
using namespace std;

Wiener_filter::Wiener_filter(void)
{
}


Wiener_filter::~Wiener_filter(void)
{
}

Mat Wiener_filter::get_recov_mat(const Mat &src,int len,float snr){
	if(len<=0)return Mat::zeros(1,1,CV_32F);
	if(snr<=0)return Mat::zeros(1,1,CV_32F);
	ori_rows = src.rows;
	ori_cols = src.cols;
    //int dft_rows = getOptimalDFTSize(src.rows);
    //int dft_cols = getOptimalDFTSize(src.cols);
    int dft_rows = src.rows;
    int dft_cols = src.cols;
	Mat dft_mat;
	int border_value = 0;
	int row_start = 5;
	int col_point = src.cols-5;
	for(int i=0;i<20;i++){
		border_value += (int)src.at<uchar>(i+row_start,col_point);
	}
	border_value /= 20;
	//cout<<"border_value :"<<border_value<<endl;
	copyMakeBorder(src,dft_mat,0,dft_rows-src.rows,0,dft_cols-src.cols,BORDER_CONSTANT,Scalar::all(border_value));

	Mat planes[] = {Mat_<float>(dft_mat), Mat::zeros(dft_mat.size(), CV_32F)}; 
	Mat  completeI;  
    merge(planes,2,completeI);
	dft(completeI,completeI);
	split(completeI,planes);

	generate_psf(dft_rows,dft_cols,len);
	gen_wiener_filter(snr);
	//cout<<planes[0].cols<<"\t"<<planes[0].rows<<endl;
	//cout<<dft_mat.cols<<"\t"<<dft_mat.rows<<endl;
	//cout<<psf.cols<<"\t"<<psf.rows<<endl;

	for(int i=0,n=dft_mat.rows;i<n;i++){
		for(int j=0,m=dft_mat.cols;j<m;j++){
			planes[0].at<float>(i,j) *= psf.at<float>(i,j);
			planes[1].at<float>(i,j) *= psf.at<float>(i,j);
		}
	}
	merge(planes,2,completeI);
	dft(completeI,completeI,DFT_INVERSE + DFT_SCALE);
	split(completeI,planes);
	double min,max;
	minMaxIdx(planes[0],&min,&max);
	for(int i=0,n=planes[0].rows;i<n;i++){
		for(int j=0,m=planes[0].cols;j<m;j++){
			planes[0].at<float>(i,j) = planes[0].at<float>(i,j)<0? 0 : planes[0].at<float>(i,j)/max*255;
		}
	}
	recov_mat = Mat_<uchar>(planes[0]);
	return recov_mat;
}

void Wiener_filter::generate_psf(int rows,int cols,int len){

	float elem_value = 1.0/len;
	//cout<<"elem_value ="<<elem_value<<endl;
	bool statue = 0;
	//make sure the len is odd
	if(0==len%2){
		len++;
		statue = 1;
	}
	Mat time_area_psf = Mat::zeros(1,cols,CV_32F);

	//creat an even symmetry list
	time_area_psf.at<float>(0,0) = elem_value;
	for(int i=0,n=(len-1)/2-1;i<n;i++){
		time_area_psf.at<float>(0,i+1) = elem_value;
		time_area_psf.at<float>(0,cols-i-1) = elem_value;
	}
	if(statue){
	//len is even ,so cut half of two point_value to make one extral point
		time_area_psf.at<float>(0,(len-1)/2) = elem_value/2;
		time_area_psf.at<float>(0,cols-(len-1)/2) = elem_value/2;
	}
	else{
	//len is even , keep the same
		time_area_psf.at<float>(0,(len-1)/2) = elem_value;
		time_area_psf.at<float>(0,cols-(len-1)/2) = elem_value;
	}

	Mat planes[] = {time_area_psf, Mat::zeros(time_area_psf.size(), CV_32F)}; 

	Mat  completeI;  
    merge(planes,2,completeI);
	dft(completeI,completeI);
	split(completeI,planes); 

	psf = Mat::zeros(rows,cols,CV_32F);
	float planes_temp;
	for(int i=0;i<cols;i++){
		planes_temp = planes[0].at<float>(0,i);
		for(int j=0;j<rows;j++){
			psf.at<float>(j,i) = planes_temp;
		}
	}
}


void Wiener_filter::gen_wiener_filter(float snr){
	float psfpoint_temp;
	for(int i=0,n=psf.rows;i<n;i++){
		for(int j=0,m=psf.cols;j<m;j++){
			psfpoint_temp = psf.at<float>(i,j);
			psf.at<float>(i,j) /= psfpoint_temp*psfpoint_temp+snr;
		}
	}
}

Mat Wiener_filter::get_orisize_mat(){
	Rect ori_rect(0,0,ori_cols,ori_rows);
	return recov_mat(ori_rect);
}



/*****************************************************************
*函数名：wnrFilter2
*功能：  实现改进的维纳滤波,采用边界延拓抑制振铃
*F(u,v)=G(u,v)*(conj(H(u,v)/(|H(u,v)|*|H(u,v)|+a))
*F(u,v)=(G(u,v)/H(u,v))*|H(u,v)|*|H(u,v)|/(|H(u,v)|*|H(u,v)|+a)
*****************************************************************/
void Wiener_filter::wnrFilter2(Mat &input, Mat &kernal, Mat &output)
{
    CvSize size = input.size();
    CvSize kernalsize = kernal.size();
    CvSize nsize;

    nsize.width=2*size.width;
    nsize.height = 2 * size.height;

    Mat y = input.clone(),y1,y2;
    Mat yy = Mat::zeros(nsize, CV_8U);
    Mat yy0 = Mat(yy, Rect(0, 0, size.width, size.height));
    Mat yy1 = Mat(yy, Rect(size.width, 0, size.width, size.height));
    Mat yy2 = Mat(yy, Rect(0, 0, nsize.width, size.height));
    Mat yy3 = Mat(yy, Rect(0, size.height, nsize.width, size.height));

    y.copyTo(yy0);
    flip(y, y1, 1);
    y1.copyTo(yy1);
    flip(yy2, y2, 0);
    y2.copyTo(yy3);

    Mat F, H, HJ, HH, G;
    Mat plane[] = { Mat_<float>(yy), Mat::zeros(yy.size(), CV_32F) };

    fft2(yy, G);
    psf2otf(kernal, H, nsize);
    conj(H, HJ);
    HH = complexMatrixMul(H, HJ);
//	double k = estimatek(input);
    HH = HH/(HH + 0.008);
    F = complexMatrixDivide(G, H);
    F = complexMatrixMul(HH, F);
    dft(F, F, DFT_INVERSE + DFT_SCALE);

    Mat FF, FJ;
    conj(F, FJ);
    FF = complexMatrixMul(F, FJ);
    split(FF, plane);
    FF = plane[0];
    split(F, plane);
    plane[0] = ((plane[0] + FF)/2);
    plane[0] = Mat(plane[0], Rect(0, 0, size.width, size.height));
    plane[0] = Mat_<uchar>(plane[0]);
    output = plane[0].clone();
}


/*****************************************************************
*函数名：wnrFilter1
*功能：  实现维纳滤波
*F(u,v)=G(u,v)*(conj(H(u,v)/(|H(u,v)|*|H(u,v)|+a))
*F(u,v)=(G(u,v)/H(u,v))*|H(u,v)|*|H(u,v)|/(|H(u,v)|*|H(u,v)|+a)
*****************************************************************/
void Wiener_filter::wnrFilter1(Mat &input, Mat &kernal, Mat &output)
{
    Mat input1 = edgetaper(input, kernal);
    Mat F, H, HJ, HH, G;
    fft2(input1, G);
    CvSize size = input1.size();
    Mat plane[] = { Mat_<float>(input1), Mat::zeros(input1.size(), CV_32F) };
    psf2otf(kernal, H, size);
    conj(H, HJ);
    HH = complexMatrixMul(H, HJ);
//	double k = estimatek(input);
    HH = HH/(HH + 0.05);
    F = complexMatrixDivide(G, H);
    F = complexMatrixMul(HH, F);
    dft(F, F, DFT_INVERSE + DFT_SCALE);

    Mat FF, FJ;
    conj(F, FJ);
    FF = complexMatrixMul(F, FJ);
    split(FF, plane);
    FF = plane[0];
    split(F, plane);
    plane[0] = ((plane[0] + FF)/2);
    plane[0] = Mat_<uchar>(plane[0]);
    output = plane[0].clone();
}


Mat Wiener_filter::edgetaper(Mat &image, Mat &psf)
{
    int irow = image.rows;
    int icol = image.cols;
    int prow = psf.rows;
    int pcol = psf.cols;
    int temp=0;

    Mat blur;
    Mat psfRowProj(1, prow, CV_32FC1);
    Mat psfColProj(1, pcol, CV_32FC1);
    Mat planes1[] = { Mat::zeros(image.size(), CV_32FC1), Mat::zeros(image.size(), CV_32FC1) };
    int i, j;

    calConv(image, psf, blur);
    split(blur, planes1);
    blur = planes1[0];

    for (j = 0; j < prow; j++)
    {
        float temp = 0;
        float* pvalue = psf.ptr<float>(j);
        for (i = 0; i < pcol; i++)
        {
            temp += pvalue[i];
        }
        psfRowProj.at<float>(j) = temp;
    }
    CvSize tsize ;
    tsize.width = icol - 1;
    tsize.height = 1;

    copyMakeBorder(psfRowProj, psfRowProj, 0, tsize.height - psfRowProj.rows, 0, tsize.width - psfRowProj.cols, BORDER_CONSTANT, Scalar::all(0));
    fft2(psfRowProj, psfRowProj);

    Mat planes2[] = { Mat::zeros(tsize, CV_32F), Mat::zeros(tsize, CV_32F) };
    Mat temp1;
    split(psfRowProj, planes2);
    magnitude(planes2[0], planes2[1], planes2[0]);
    planes2[0] = planes2[0].mul(planes2[0]);
    dft(planes2[0], temp1, DFT_INVERSE + DFT_SCALE);
    split(psfRowProj, planes2);
    Mat beta[] = { Mat::zeros(1, icol, CV_32F), Mat::zeros(1, irow, CV_32F) };
    Mat temp2(beta[0], Rect(0, 0, icol - 1, 1));
    Mat temp3(beta[0], Rect(0, 0, 1, 1));
    Mat temp4(beta[0], Rect(icol - 1, 0, 1, 1));

    planes2[0].copyTo(temp2);
    temp3.copyTo(temp4);

    for (i = 0; i < pcol; i++)
    {
        float temp = 0;
        for (j = 0; j < prow; j++)
        {
            temp += psf.at<float>(j, i);
        }
        psfColProj.at<float>(i) = temp;
    }
    tsize.width = irow - 1;
    tsize.height = 1;

    copyMakeBorder(psfColProj, psfColProj, 0, tsize.height - psfColProj.rows, 0, tsize.width - psfColProj.cols, BORDER_CONSTANT, Scalar::all(0));
    fft2(psfColProj, psfColProj);

    Mat planes3[] = { Mat::zeros(tsize, CV_32F), Mat::zeros(tsize, CV_32F) };
    split(psfColProj, planes3);
    magnitude(planes2[0], planes2[1], planes2[0]);
    planes2[0] = planes2[0].mul(planes2[0]);
    dft(planes2[0], temp1, DFT_INVERSE + DFT_SCALE);
    dft(temp1, temp1, DFT_INVERSE + DFT_SCALE);
    split(psfColProj, planes3);
    Mat temp5(beta[1], Rect(0, 0, icol - 1, 1));
    Mat temp6(beta[1], Rect(0, 0, 1, 1));
    Mat temp7(beta[1], Rect(icol - 1, 0, 1, 1));

    planes3[0].copyTo(temp5);
    temp6.copyTo(temp7);

    Mat alpha(image.size(), CV_32F);
    beta[0] = 1 - beta[0];
    beta[1] = 1 - beta[1];
    transpose(beta[0], beta[0]);
    alpha = beta[0] * beta[1];
    Mat image1 = Mat_<float>(image);

    Mat J = alpha.mul(image1) + (1 - alpha).mul(blur);

    J = Mat_<uchar>(J);
    return J;
}
