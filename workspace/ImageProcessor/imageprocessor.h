#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QUdpSocket>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define ADDRESS "127.0.0.1"
//#define ADDRESS "192.168.2.100"
#define PORT 8888
#define DATAGRAM_SIZE 512

class ImageProcessor : public QObject
{
    Q_OBJECT
private:
    QUdpSocket* socket;
    CvCapture* capture;
    IplImage* image;
    qint32 imageId;
    QHostAddress address;

public:
    explicit ImageProcessor(QObject* parent = 0);
    void run(void);
    void sendImage(void);
};

#endif // IMAGEPROCESSOR_H
