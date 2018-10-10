#include <JHCap.h>
#include "capturethread.h"
#include "Windows.h"

#include <QDebug>

CaptureThread::CaptureThread(QObject *parent) :
    QThread(parent)
{
    work = false;
    term = false;
    index=0;
    sendFlag = false;

    for(int i = 0; i < 256; i++)
    {
       grayColourTable.append(qRgb(i, i, i));
    }
}

void CaptureThread::run()
{
    forever
    {
        int width, height, len, size;

        if(work)
        {
            CameraGetImageSize(index,&width, &height);
            CameraGetImageBufferSize(index,&len, CAMERA_IMAGE_RAW8);
            CameraGetISPImageBufferSize(index,&size,width,height,CAMERA_IMAGE_RGB24);
            unsigned char *buffer = new unsigned char[len];
            unsigned char *ISPbuf = new unsigned char[size];
            if(CameraQueryImage(index,buffer, &len, CAMERA_IMAGE_RAW8)==API_OK)
            {
                if(term) break;
              // QImage img(buffer, width, height, QImage::Format_Indexed8);
               // img.setColorTable(grayColourTable);
                if(CameraISP(index,buffer,ISPbuf,width,height,CAMERA_IMAGE_RGB24)==API_OK)
                {
                    QImage img(ISPbuf, width, height, QImage::Format_RGB888);
//                    delete [] ISPbuf;
                    if(sendFlag == false){
                        emit captured(img, ISPbuf);
                    }
                    if(sendFlag == true)
                    {
                        usleep(50000);
//                        qDebug()<<"send";
                        emit send(img, ISPbuf);
                    }
                }

            }
            delete [] buffer;
        } else usleep(1000);
        if(term) break;
    }
}

void CaptureThread::stream()
{
    work = true;
}
void CaptureThread::setIndex(int index)
{
    this->index=index;
}

void CaptureThread::pause()
{
    work = false;
}

void CaptureThread::stop()
{
    work = false;
    term = true;
}

void CaptureThread::setSendFlag(){
    sendFlag = true;
}
