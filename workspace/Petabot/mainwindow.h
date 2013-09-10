#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "imagereceiver.h"

#include "winsock2.h"
#include <windows.h>

#define WIdTH 640
#define HEIGHT 480

class ImageReceiver;

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
    ImageReceiver *worker;

public slots:
    void showImage();
};

#endif // MAINWINDOW_H
