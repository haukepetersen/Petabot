#ifndef IMAGERECIEVER_H
#define IMAGERECIEVER_H

#include <QThread>
#include <QUdpSocket>

#include <stdio.h>

#ifdef Q_OS_WIN
#include "winsock2.h"
#include <windows.h>
#endif

#include "mainwindow.h"

#define MAXPENDING 5
#define PORT 8888

#define WIDTH 640
#define HEIGHT 480

#define BUFFER_SIZE 4

class MainWindow;

class ImageReceiver : public QThread
{
    Q_OBJECT

public:
    ImageReceiver(MainWindow *parent = 0);
    uchar* getImage();

private:
    MainWindow *parent;
    QUdpSocket *socket;
    uchar image[BUFFER_SIZE][WIDTH * HEIGHT * 3];
    qint32 imageId;
    qint32 receivedBytes[BUFFER_SIZE];

    void initializeSocket(void);
    void run(void);

    void processDatagram(QByteArray datagram, qint32 imageId, qint32 chunkId);

signals:
    void imageChanged();

public slots:
    void readDatagram();
};

#endif // IMAGERECIEVER_H
