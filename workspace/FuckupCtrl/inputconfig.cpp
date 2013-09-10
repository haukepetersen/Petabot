#include "inputconfig.h"
#include "ui_inputconfig.h"
#include "QDebug"
#include "SDL/SDL.h"

InputConfig::InputConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputConfig),
    joystick(NULL)
{
    ui->setupUi(this);
//    ui->slider->setScale(0.0d, 1.0d, 0.05d);
//    ui->slider->setRange(0.0d, 1.0d, 0.05d);
    initJoy();
    eventTimer = new QTimer(this);
    eventTimer->setInterval(100);
    connect(eventTimer, SIGNAL(timeout()), this, SLOT(joyTimer()));
}

InputConfig::~InputConfig()
{
    delete eventTimer;
    SDL_JoystickClose(joystick);
    SDL_Quit();
    delete ui;
}

void InputConfig::initJoy(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    int joyCount = SDL_NumJoysticks();
    for (int i =0; i < joyCount; i++) {
        ui->joysticks->addItem(tr("Joystick/pad ") + QString::number(i));
    }
    if (joyCount > 0) {
        ui->open->setEnabled(true);
    } else {
        ui->open->setEnabled(false);
    }
}

void InputConfig::on_open_clicked(void)
{
    if (ui->joysticks->currentIndex() < SDL_NumJoysticks()) {
        joystick = SDL_JoystickOpen(ui->joysticks->currentIndex());
        if (joystick != NULL) {
            eventTimer->start();
            ui->open->setEnabled(false);
            ui->joyStatus->setText(tr("Opened"));
        }
    }

}

void InputConfig::joyTimer(void)
{
    SDL_Event *event = new SDL_Event();

    while (SDL_PollEvent(event) > 0) {
        if (event != NULL && event->type == SDL_JOYAXISMOTION) {
            if (event->jaxis.which == 0) {
                if (event->jaxis.axis == 0) {
                    ui->axis0->setText(QString::number(event->jaxis.value));
                } else if (event->jaxis.axis == 1) {
                    ui->axis1->setText(QString::number(event->jaxis.value));
                } else if (event->jaxis.axis == 2) {
                    ui->axis4->setText(QString::number(event->jaxis.value));
                } else if (event->jaxis.axis == 3) {
                    ui->axis5->setText(QString::number(event->jaxis.value));
                }

            }
        }
    }
}

void InputConfig::on_btnStop_clicked(void)
{
    ui->compass->setValue(120.0d);
    ui->slider->setValue(0.0d);
}

void InputConfig::on_btnHalf_clicked(void)
{
    ui->slider->setValue(0.5d);
    ui->compass->setValue(240.0d);
}

void InputConfig::on_btnFull_clicked(void)
{
    ui->slider->setValue(1.0d);
    ui->compass->setValue(0.0d);
}


