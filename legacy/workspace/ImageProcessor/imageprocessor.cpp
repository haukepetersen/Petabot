#include "imageprocessor.h"

ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent)
{;
    socket = new QUdpSocket(this);
    capture = cvCaptureFromCAM(0);
#ifdef QT_DEBUG
    cvNamedWindow("window");
#endif
    imageId = 0;
    address = QHostAddress(ADDRESS);
}

void ImageProcessor::run()
{
    while(TRUE)
    {
        image = cvQueryFrame(capture);
#ifdef QT_DEBUG
        cvShowImage("window", image);
#endif
        cvCvtColor(image, image, CV_BGR2RGB);
        if(image != NULL)
        {
            sendImage();
        }
    }
}

void ImageProcessor::sendImage()
{
    int i = 0;
    int n = 0;
    char header[8];
    memcpy(header, &imageId, sizeof(qint32));
    QByteArray data = QByteArray::fromRawData(image->imageData, image->imageSize);
    QByteArray tmp = data.mid(i, DATAGRAM_SIZE - (2 * sizeof(qint32)));
    while (tmp.size() > 0) {
        memcpy(header + sizeof(qint32), &i, sizeof(qint32));
        tmp.prepend(header, 8);
        n = socket->writeDatagram(tmp, address, PORT);
        i = i + n - (2 * sizeof(qint32));
        tmp = data.mid(i, DATAGRAM_SIZE - (2 * sizeof(qint32)));
    }
    qDebug() << "imageId:" << imageId << "data size:" << data.size() << "send " << i << " of " << image->imageSize;
    imageId++;

/*
    qint32 headerSize = sizeof(qint32) * 2;
    qint32 packetSize = DATAGRAM_SIZE - headerSize;
    qint32 imageSize = image->imageSize;
    qint32 packetAmount = (imageSize + packetSize - 1) / packetSize;
    qint32 lastPacketSize = image->imageSize % packetSize;
    qint32 sentBytes = 0;
    for(qint32 i = 0; i < packetAmount; i++)
    {
        char buffer[DATAGRAM_SIZE];
        memcpy(buffer, &imageId, sizeof(qint32));
        memcpy(buffer + sizeof(qint32), &i, sizeof(qint32));
        qint32 currentPacketSize = i < packetAmount - 1 ? packetSize : lastPacketSize;
        memcpy(buffer + sizeof(qint32) * 2, image->imageData + (i * packetSize), currentPacketSize);
        qint32 datagramSize = (i < (packetAmount - 1) ? packetSize + headerSize : lastPacketSize + headerSize);
        sentBytes += socket->writeDatagram(buffer, datagramSize, address, PORT);
    }
    qDebug() << "image sent with imageId: " << imageId << "num packets" << packetAmount << "packet size" << packetSize << "send bytes" << sentBytes << "image size" << imageSize;
    imageId++;
 */
}
