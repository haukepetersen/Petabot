#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->gvImage->setScene(scene);
    worker = new ImageReceiver();
    QObject::connect(worker, SIGNAL(imageChanged()), this, SLOT(showImage()));
    worker->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImage()
{
    scene->clear();
    uchar *image = worker->getImage();
    scene->addPixmap(QPixmap::fromImage(QImage(image, WIDTH, HEIGHT, QImage::Format_RGB888)));
}
