#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject *parent = 0);

public:
    void run();
    void setIndex(int index);
    void stream();
    void pause();
    void stop();
    void setSendFlag();

signals:
    void captured(QImage img, unsigned char *buffer);
    void send(QImage img, unsigned char *buffer);
private:
    int index;
    bool term;



public:
    bool work;
    bool sendFlag;

    QVector<QRgb> grayColourTable;

public slots:

};

#endif // CAPTURETHREAD_H
