#ifndef PETA_H
#define PETA_H

#include <QMainWindow>

namespace Ui {
class Peta;
}

class Peta : public QMainWindow
{
    Q_OBJECT

public:
    explicit Peta(QWidget *parent = 0);
    ~Peta();

private:
    Ui::Peta *ui;
};

#endif // PETA_H
