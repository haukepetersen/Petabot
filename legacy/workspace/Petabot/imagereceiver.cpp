#include "imagereceiver.h"

ImageReceiver::ImageReceiver(MainWindow *parent) : QThread(parent)
{
    imageId = 0;
    for(int i = 0; i < BUFFER_SIZE; i++) {
        receivedBytes[i] = 0;
    }
    initializeSocket();
}

void ImageReceiver::initializeSocket(void)
{
    socket = new QUdpSocket(this);
    socket->bind(PORT);

    //Workaround because of to small recv buffer
    #ifdef Q_OS_WIN
    int v = -1;
    if (::setsockopt(socket->socketDescriptor(), SOL_SOCKET, SO_RCVBUF, (char *)&v, sizeof(v)) == -1) {
        exit();  // error!
    }
    #endif

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
}

void ImageReceiver::run(void)
{
    exec();
}

void ImageReceiver::readDatagram(void)
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        qint32 imageId;
        qint32 chunkId;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());
        memcpy(&imageId, datagram.data(), sizeof(qint32));
        memcpy(&chunkId, (datagram.data() + sizeof(qint32)), sizeof(qint32));
        processDatagram(datagram.mid(sizeof(qint32) * 2), imageId, chunkId);
    }
}

uchar* ImageReceiver::getImage()
{
    qint32 id = (imageId - 1) % BUFFER_SIZE;
    return image[id];
}

void ImageReceiver::processDatagram(QByteArray imageData, qint32 imageId, qint32 chunkId)
{
    qint32 oldId = this->imageId % BUFFER_SIZE;

    if(imageId > this->imageId)
    {
        //qDebug() << "imageId" << this->imageId << "received" << receivedBytes[oldId] << "of" << image[oldId]->imageSize;
        receivedBytes[oldId] = 0;
        this->imageId = imageId;
        emit imageChanged();
    }

    qint32 id = imageId % BUFFER_SIZE;

    memcpy(image[id] + chunkId, imageData.data(), imageData.size());
    receivedBytes[id] += imageData.size();
}
