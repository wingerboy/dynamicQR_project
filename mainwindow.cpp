#include <JHCap.h>
//#include <vld.h>
#include <winsock2.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include"qpainter.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;
using namespace cv;

bool sendFlag = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    CameraParaSet();//设置相机的各项参数
    connectServer();
    readKernel();

//    triggerTimer = new QTimer(this);
    show_thread = new CaptureThread(this);
    send_thread = new CaptureThread(this);
    send_thread->setSendFlag();

    connect(show_thread, SIGNAL(captured(QImage,unsigned char*)),this, SLOT(showProcess(QImage,unsigned char*)), Qt::DirectConnection);
    connect(send_thread, SIGNAL(send(QImage,unsigned char*)),this, SLOT(sendProcess(QImage,unsigned char*)), Qt::DirectConnection);
//    connect(triggerTimer, SIGNAL(timeout()),this, SLOT(setSendFlag()), Qt::DirectConnection);//定时器抓拍连接槽
    show_thread->start();
    show_thread->stream();
    send_thread->start();
    send_thread->stream();

//    triggerTimer->start(10);
}

MainWindow::~MainWindow()
{
    delete show_thread;
    delete send_thread;
//    delete triggerTimer;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * e)
{
    show_thread->stop();
    show_thread->wait();
    send_thread->stop();
    send_thread->wait();
    CameraFree(0);
    closesocket(sockClient); //关闭套接字
    WSACleanup();
    QMainWindow::closeEvent(e);
}


void MainWindow::sendProcess(QImage img, unsigned char *buffer)
{

    if(send_thread->work)
    {
        sendQrImage(img);
        if(!imgROI.empty()){
            imageProcess(imgROI);
        }
//        recvImg = receiveServerImg(img);
        delete []buffer;
    }

}

void MainWindow::imageProcess(Mat qrImg){
    int burLen;
    Mat circler, qrImgGray, cuter;

    cvtColor(qrImg, qrImgGray, CV_BGR2GRAY);
    imwrite("qrImg.jpg", qrImgGray);
    burLen = blurlener.get_blurlen(qrImgGray);
    qDebug()<<"burLen: "<<burLen;
    circler = Xunhuan(qrImgGray);
//    imwrite("circler.jpg", circler);
///*
    if(burLen == 10){
        winer.wnrFilter2(circler, kernel_10, circler);
    }
    else if(burLen == 11){
        winer.wnrFilter2(circler, kernel_11, circler);
    }
    else if(burLen == 12){
        winer.wnrFilter2(circler, kernel_12, circler);
    }
    else if(burLen == 13){
        winer.wnrFilter2(circler, kernel_13, circler);
    }
    else if(burLen == 14){
        winer.wnrFilter2(circler, kernel_14, circler);
    }
    else if(burLen == 15){
        winer.wnrFilter2(circler, kernel_15, circler);
    }
    else if(burLen == 16){
        winer.wnrFilter2(circler, kernel_16, circler);
    }
    else if(burLen == 17){
        winer.wnrFilter2(circler, kernel_17, circler);
    }
    else if(burLen == 18){
        winer.wnrFilter2(circler, kernel_18, circler);
    }
    else{
        circler = winer.get_recov_mat(circler,burLen,0.01);
    }
//*/
//    circler = winer.get_recov_mat(circler,burLen,0.01);
    circler = Cutter(circler);
    Mat binaryImg ;
    threshold(circler, binaryImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    QString decode_result_cuter_ker = decoder.decodeImage(cvMat2QImage(binaryImg),-1,-1,true);

    qDebug()<<"decode_result_cuter_ker="<<decode_result_cuter_ker.right(5);
    imwrite("binaryImg_kernel.jpg", binaryImg);

//    imwrite("tempMat.jpg", tempMat);
//    cuter = Cutter(tempMat);
//    imwrite("cuter.jpg", cuter);
//    Mat binaryImg ;
//    threshold(cuter, binaryImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//    imwrite("binaryImg.jpg", binaryImg);

//    dilate(binaryImg,binaryImg,Mat(1,1,CV_8U),Point(-1,-1),1);
//    imwrite("dilate.jpg", binaryImg);
//    QString decode_result_cuter = decoder.decodeImage(cvMat2QImage(binaryImg),-1,-1,true);
//    erode(binaryImg,binaryImg,Mat(2,2,CV_8U),Point(-1,-1),1);
//    imwrite("erode.jpg", binaryImg);
//    QString decode_result_cuter1 = decoder.decodeImage(cvMat2QImage(binaryImg),-1,-1,true);

//    qDebug()<<"decode_result_cuter="<<decode_result_cuter.right(5);
//    qDebug()<<"Decode_result_cuter="<<decode_result_cuter1.right(5);
//    if(decode_result_cuter != ""){
//        this->ui->resultLabel->setText(decode_result_cuter.right(5));
//    }
//    bool goon = qrRecover.try_recovery(cuter);
//    if(goon){
//        recoverImg = qrRecover.get_recovQRcode();
//        imwrite("recoverImg.jpg", recoverImg);
//        Mat recoverImg2 = qrRecover.get_recovQRcodeImg();
//        Mat recoverImg3 = Mat::zeros(cvSize(recoverImg.rows,recoverImg.cols),CV_8UC3);
//        cvtColor(recoverImg2, recoverImg3, CV_GRAY2BGR);
//        Mat recoverImgcp = recoverImg2.clone();
//        QImage decodeImg3 = cvMat2QImage(recoverImg3);
//        QImage decodeImg = decodeImg3.copy();
//        QImage decodeImg2 = cvMat2QImage(recoverImg2);

//        decoder.setDecoder(QZXing::DecoderFormat_DATA_MATRIX|QZXing::DecoderFormat_QR_CODE);
//        imwrite("decodeImg.jpg", QImage2cvMat(decodeImg));
//        QString decode_result = decoder.decodeImage(decodeImg,-1,-1,true);
//        QString decode_result_cuter = decoder.decodeImage(cvMat2QImage(cuter),-1,-1,true);
//        qDebug()<<"decode_result="<<decode_result;
//        qDebug()<<"decode_result_cuter="<<decode_result_cuter;
//        if(decode_result != ""){
//            this->ui->resultLabel->setText(decode_result);
//        }
//    }
//    else{
//        QString decode_result_cuter = decoder.decodeImage(cvMat2QImage(cuter),-1,-1,true);
//        qDebug()<<"decode_result="<<decode_result_cuter;
//        if(decode_result_cuter != ""){
//                this->ui->resultLabel->setText(decode_result_cuter);
//        }
//    }
}


void MainWindow::showProcess(QImage img, unsigned char *buffer)
{
    if(show_thread->work)
    {
        this->ui->cameraLabel->setAttribute(Qt::WA_DeleteOnClose);
        this->ui->cameraLabel->setPixmap(QPixmap::fromImage(img).scaled(this->ui->cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));//按照原图设置的比例显示在QLabel控件上

        if(!imgROI.empty()){
            this->ui->frameLabel->setAttribute(Qt::WA_DeleteOnClose);
            this->ui->frameLabel->setPixmap(QPixmap::fromImage(cvMat2QImage(imgROI)).scaled(this->ui->frameLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

//        delete []buffer;
    }
    delete []buffer;
}


/*设置相机的各项参数*/
void MainWindow::CameraParaSet()
{
    /*相机的各项参数*/
    int device_id = 0;//相机的序号
    int index = 1;//设置相机的分辨率为index,即1024X768
    //int *width;
    //int *height;//分辨率宽度width和高度height
    int gain = 15;//设置相机的增益
    int exposure = 180;//设置相机的曝光时间（0-4000）
    bool awb = true;//设置相机的自动白平衡

    CameraInit(0);

    CameraSetResolution(device_id, index, 0, 0);//设置相机的分辨率为 index,返回此分辨率宽度width和高度height
    CameraSetGain(device_id, gain);//设置相机的增益
    CameraSetExposure(device_id, exposure);//设置相机曝光值（这和曝光时间不同）
    CameraSetAWB(device_id, awb);//设置相机的白平衡

    CameraSetROI(device_id, 600, 300, 800, 600);
    if(CameraSetHighspeed(device_id , false) == API_OK){
        qDebug()<<"已设置相机为低速模式" ; //设置相机的速度为低速模式
    }
}


void MainWindow::connectServer()
{
    WSADATA wsaData;

    SOCKADDR_IN addrServer;//服务端地址
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //新建客户端socket
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
//    int ret=ioctlsocket(sockClient FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。
//    if(ret==SOCKET_ERROR)//设置失败。
//    {
//        qDebug()<<"设置失败" ;
//    }
    //定义要连接的服务端地址
//    addrServer.sin_addr.S_un.S_addr = inet_addr("192.168.1.102");//本机ip
    addrServer.sin_addr.S_un.S_addr = inet_addr("125.217.249.114");//服务器ip
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(8005);//连接端口
    //连接到服务端
    int result = ::connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
    if (result == SOCKET_ERROR)
    {
        printf("connect() failed!\n");
        closesocket(sockClient); //关闭套接字
        WSACleanup();
    }
}



void MainWindow::sendQrImage(QImage img)
{
    Mat sendImg = QImage2cvMat(img);
//    imshow("sendImg", sendImg);

    imencode(".jpg", sendImg, data_encode);
    uint len_encode = data_encode.size();
    string len = to_string(len_encode);
    int length = len.length();
//    qDebug()<<"send image size:"<<len_encode;
    for (int i = 0; i < 16 - length; i++)
    {
        len = len + " ";
    }
    //发送数据大小
    len.shrink_to_fit();
    send(sockClient, len.c_str(), strlen(len.c_str()), 0);
    string str(data_encode.begin(),data_encode.end());
    const char * buf=str.c_str();
    send(sockClient, buf, len_encode, 0);
//    char send_char;
//    //发送图片数据
//    for (uint i = 0; i < len_encode; i++)
//    {
//        send_char = data_encode[i];
//        send(sockClient, &send_char, 1, 0);
//    }
    qDebug()<<"send have done!!!";
    receiveServerImg(img);

}

void MainWindow::receiveServerImg(QImage image)
{
    Mat recvMat = QImage2cvMat(image);
    std::vector<std::string> resultVec;
    char recvBuf[20];
    string recvStr;

    if(recv(sockClient, recvBuf, 20, 0))
    {
//        printf("%s\n", recvBuf);
        recvStr = recvBuf;
//        qDebug()<<"string: "<<QString::fromStdString(recvStr);

        const char *d = ";";
        char *p;

        char *recvBufSplit = strtok(recvBuf, " ");//先删除后面多余的空格
//        qDebug()<<"not recevice imag: "<<recvBufSplit<<"\n";
//        qDebug()<<"not recevice imag size: "<<strlen(recvBufSplit)<<"\n";
        if(strlen(recvBufSplit) <= 2){
//            qDebug()<<"not recevice imag: "<<recvBufSplit<<"\n";
            return;
        }
//        cout<<"recvBufSplit:"<<recvBufSplit;
        p = strtok(recvBufSplit, d);

        while(p)
        {
//            printf("split:%s\n",p);
            resultVec.push_back(string(p));
            p=strtok(NULL, d);
        }
//        printf("x:%s\n",string(resultVec[0]));
        qDebug()<<"x:"<<atoi(resultVec[3].c_str());
    }
    //剪裁图像
    float w_h_rate = (float(atoi(resultVec[2].c_str()) - atoi(resultVec[0].c_str()))) / (float(atoi(resultVec[3].c_str())) - atoi(resultVec[1].c_str()));
    if (w_h_rate > 0.8 && w_h_rate < 1.2){
        recvMat(Rect(atoi(resultVec[0].c_str()), atoi(resultVec[1].c_str()),
                atoi(resultVec[2].c_str()) - atoi(resultVec[0].c_str()),
                atoi(resultVec[3].c_str()) - atoi(resultVec[1].c_str()))).copyTo(imgROI);
    }
//    recvMat(Rect(atoi(resultVec[0].c_str()), atoi(resultVec[1].c_str()),
//            atoi(resultVec[2].c_str()) - atoi(resultVec[0].c_str()),
//            atoi(resultVec[3].c_str()) - atoi(resultVec[1].c_str()))).copyTo(imgROI);
    recvMat.release();
//    imwrite("imgROI.jpg", imgROI);
//    imshow("imgROI", imgROI);
    qDebug()<<"recv have done";
    delete [] recvBuf;
//    return recvMat;
}


QImage MainWindow::cvMat2QImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
//qDebug() << "CV_8UC1";
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
//qDebug() << "CV_8UC3";
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

Mat MainWindow::QImage2cvMat(QImage image)
{
    Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
//qDebug() << "Format_ARGB32_Premultiplied";
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
//qDebug() << "Format_RGB888";
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
//qDebug() << "Format_Indexed8";
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}


cv::Mat MainWindow::Xunhuan(cv::Mat &src){
    Mat temp = Mat::zeros(src.rows*2,src.cols*2, CV_8U);
    uchar char_temp;
    for (int i=0,n=src.rows;i<n;i++){
        for (int j=0,m=src.cols;j<m;j++){
            char_temp = src.at<uchar>(i,j);
            temp.at<uchar>(i,j) = char_temp;
            temp.at<uchar>(src.rows*2-i-1,j) = char_temp;
            temp.at<uchar>(i,src.cols*2-j-1) = char_temp;
            temp.at<uchar>(src.rows*2-i-1,src.cols*2-j-1) = char_temp;
        }
    }
    return temp;
}


cv::Mat MainWindow::Cutter(cv::Mat &src){
    Mat temp = Mat::zeros(src.rows/2,src.cols/2, CV_8U);
    for (int i=0,n=src.rows/2;i<n;i++){
        for (int j=0,m=src.cols/2;j<m;j++){
            temp.at<uchar>(i,j) = src.at<uchar>(i,j);
        }
    }
    return temp;
}


void MainWindow::imageInit(Mat &input,Mat &output)
{
    int m = getOptimalDFTSize(input.rows);
    int n = getOptimalDFTSize(input.cols);

    copyMakeBorder(input, output, 0, m - input.rows, 0, n - input.cols, BORDER_CONSTANT, Scalar::all(0));
}


void MainWindow::readKernel()
{
    kernel_10 = imread("kernels/len_10.png",0); //读取卷积核
    imageInit(kernel_10, kernel_10);
    cv::Scalar imgScalar_10 = cv::sum(kernel_10);
    double sumValue_10 = imgScalar_10[0];
    kernel_10.convertTo(kernel_10, CV_32F);
    cv::divide(kernel_10, sumValue_10, kernel_10, 1);

    kernel_11 = imread("kernels/len_11.png",0); //读取卷积核
    imageInit(kernel_11, kernel_11);
    cv::Scalar imgScalar_11 = cv::sum(kernel_11);
    double sumValue_11 = imgScalar_11[0];
    kernel_11.convertTo(kernel_11, CV_32F);
    cv::divide(kernel_11, sumValue_11, kernel_11, 1);

    kernel_12 = imread("kernels/len_12.png",0); //读取卷积核
    imageInit(kernel_12, kernel_12);
    cv::Scalar imgScalar_12 = cv::sum(kernel_12);
    double sumValue_12 = imgScalar_12[0];
    kernel_12.convertTo(kernel_12, CV_32F);
    cv::divide(kernel_12, sumValue_12, kernel_12, 1);

    kernel_13 = imread("kernels/len_13.png",0); //读取卷积核
    imageInit(kernel_13, kernel_13);
    cv::Scalar imgScalar_13 = cv::sum(kernel_13);
    double sumValue_13 = imgScalar_13[0];
    kernel_13.convertTo(kernel_13, CV_32F);
    cv::divide(kernel_13, sumValue_13, kernel_13, 1);

    kernel_14 = imread("kernels/len_14.png",0); //读取卷积核
    imageInit(kernel_14, kernel_14);
    cv::Scalar imgScalar_14 = cv::sum(kernel_14);
    double sumValue_14 = imgScalar_14[0];
    kernel_14.convertTo(kernel_14, CV_32F);
    cv::divide(kernel_14, sumValue_14, kernel_14, 1);

    kernel_15 = imread("kernels/len_15.png",0); //读取卷积核
    imageInit(kernel_15, kernel_15);
    cv::Scalar imgScalar_15 = cv::sum(kernel_15);
    double sumValue_15 = imgScalar_15[0];
    kernel_15.convertTo(kernel_15, CV_32F);
    cv::divide(kernel_15, sumValue_15, kernel_15, 1);

    kernel_16 = imread("kernels/len_16.png",0); //读取卷积核
    imageInit(kernel_16, kernel_16);
    cv::Scalar imgScalar_16 = cv::sum(kernel_16);
    double sumValue_16 = imgScalar_16[0];
    kernel_10.convertTo(kernel_16, CV_32F);
    cv::divide(kernel_16, sumValue_16, kernel_16, 1);

    kernel_17 = imread("kernels/len_17.png",0); //读取卷积核
    imageInit(kernel_17, kernel_17);
    cv::Scalar imgScalar_17 = cv::sum(kernel_17);
    double sumValue_17 = imgScalar_17[0];
    kernel_17.convertTo(kernel_17, CV_32F);
    cv::divide(kernel_17, sumValue_17, kernel_17, 1);

    kernel_18 = imread("kernels/len_18.png",0); //读取卷积核
    imageInit(kernel_18, kernel_18);
    cv::Scalar imgScalar_18 = cv::sum(kernel_18);
    double sumValue_18 = imgScalar_18[0];
    kernel_18.convertTo(kernel_18, CV_32F);
    cv::divide(kernel_18, sumValue_18, kernel_18, 1);
}
