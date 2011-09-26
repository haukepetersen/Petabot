#include "imagereciever.h"

ImageReciever::ImageReciever(MainWindow *parent) : QThread(parent)
{
    this->parent = parent;   
    image = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
}

void ImageReciever::dieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

void ImageReciever::initializeSocket()
{
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        dieWithError("socket() failed");

    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = 0;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))
            < 0)
        dieWithError("bind() failed");

    if (listen(serverSocket, MAXPENDING) < 0)
        dieWithError("listen() failed");

    if ((clientSocket = accept(serverSocket, NULL, NULL)) < 0)
        dieWithError("accept() failed");
}

void ImageReciever::readImage()
{
    int imgsize = image->imageSize;
    unsigned int bytesReceived = 0;
    char sockdata[imgsize];

    int i;

    for (i = 0; i < imgsize; i += bytesReceived) {
        if ((bytesReceived = recv(clientSocket, sockdata + i, imgsize - i, 0))
                == -1) {
            dieWithError("recv failed");
        }
    }

    image->imageData = sockdata;
    if(parent != 0)
    {
        parent->setImage(image);
        emit imageChanged();
    }
}

void ImageReciever::run()
{
    initializeSocket();
    while(true)
    {
        readImage();
    }
}
