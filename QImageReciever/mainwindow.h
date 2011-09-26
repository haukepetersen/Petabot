#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "imagereciever.h"

#define WIDTH 640
#define HEIGHT 480

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    IplImage *image;

public slots:
    void showImage();
    void setImage(IplImage *newImage);
};

#endif // MAINWINDOW_H
