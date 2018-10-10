#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include "capturethread.h"
#include <winsock2.h>
#include "Blurlen_detect.h"
#include "QRcodeCuter.h"
#include "Wiener_filter.h"
#include "QRcode_recovery.h"
#include <QZXing.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

private:
    void CameraParaSet();
    QImage cvMat2QImage(const cv::Mat& mat);
    cv::Mat QImage2cvMat(QImage image);
    void connectServer();
    void sendQrImage(QImage image);
    void receiveServerImg(QImage image);
    void imageProcess(cv::Mat qrImg);
    cv::Mat Xunhuan(cv::Mat &src);
    cv::Mat Cutter(cv::Mat &);
    void imageInit(Mat &input,Mat &output);
    void readKernel();

private:
    Ui::MainWindow *ui;
    CaptureThread *show_thread;
    CaptureThread *send_thread;
    SOCKET sockClient;//客户端Socket
    std::vector<uchar> data_encode;
    Blurlen_detect blurlener;
    QRcodeCuter qrCuter;
    Wiener_filter winer;
    QRcode_recovery qrRecover;
    QZXing decoder;

    cv::Mat kernel_10, kernel_11, kernel_12, kernel_13, kernel_14, kernel_15, kernel_16, kernel_17, kernel_18;

public:
//    std::vector<std::string> resultVec;
    cv::Mat imgROI;


private slots:
    void showProcess(QImage img, unsigned char *buffer);//读取帧的槽函数
    void sendProcess(QImage img, unsigned char *buffer);
//    void setSendFlag();

};

#endif // MAINWINDOW_H
