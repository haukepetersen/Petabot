#include "peta.h"
#include "ui_peta.h"
#include "joydev.h"
#include "QDebug"

Peta::Peta(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Peta)
{
    ui->setupUi(this);

    qDebug() << "Hello HiHi!";

    Joydev foo;


    qDebug() << "Joystick update" << foo.getJoydevs().size();
    if (!foo.isOpen()) {
        foreach (QString s, foo.getJoydevs()) {
            qDebug() << s;
        }
    }

    foo.open(0);
    qDebug() << "opened";

    qDebug() << "Axes:" << foo.getAxes();
    qDebug() << "Buttons: " << foo.getButtons();


}

Peta::~Peta()
{
    delete ui;
}
