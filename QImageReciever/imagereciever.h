#ifndef IMAGERECIEVER_H
#define IMAGERECIEVER_H

#include <QThread>

#include <opencv/cv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include "mainwindow.h"

#define MAXPENDING 5
#define PORT 8333

#define WIDTH 640
#define HEIGHT 480

class MainWindow;

class ImageReciever : public QThread
{
    Q_OBJECT

public:
    ImageReciever(MainWindow *parent = 0);

private:
    MainWindow *parent;
    IplImage *image;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int serverSocket;
    int clientSocket;
    unsigned int clntLen;

    void dieWithError(char *errorMessage);
    void initializeSocket();
    void readImage();
    void run();

signals:
    void imageChanged();
};

#endif // IMAGERECIEVER_H
