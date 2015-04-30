#include "peta.h"
#include "ui_peta.h"

Peta::Peta(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Peta)
{
    ui->setupUi(this);
}

Peta::~Peta()
{
    delete ui;
}
