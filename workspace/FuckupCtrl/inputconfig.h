#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H

#include <QWidget>
#include <QTimer>
#include <SDL/SDL.h>

namespace Ui {
class InputConfig;
}

class InputConfig : public QWidget
{
    Q_OBJECT

private:
    Ui::InputConfig *ui;
    QTimer *eventTimer;
    SDL_Joystick *joystick;
    
public:
    explicit InputConfig(QWidget *parent = 0);
    ~InputConfig();

private:
    void initJoy(void);

private slots:
    void on_open_clicked(void);
    void joyTimer(void);
    void on_btnStop_clicked(void);
    void on_btnHalf_clicked(void);
    void on_btnFull_clicked(void);
};

#endif // INPUTCONFIG_H
