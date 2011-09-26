#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->gvImage->setScene(scene);
    ImageReciever *worker = new ImageReciever(this);
    connect(worker, SIGNAL(imageChanged()), this, SLOT(showImage()));
    worker->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImage()
{
    scene->clear();
    cvCvtColor(image, image, CV_BGR2RGB);
    scene->addPixmap(QPixmap::fromImage(QImage((uchar *)image->imageData, image->width, image->height, QImage::Format_RGB888)));
}

void MainWindow::setImage(IplImage *newImage)
{
    image = newImage;
}
