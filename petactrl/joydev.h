#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QThread>
#include "SDL/SDL.h"

class Joydev : public QObject
{
	Q_OBJECT

private:
	QThread thread;
	QTimer timer;
	int availableDevices;
	SDL_Joystick *device;
	bool handlingEvents;

public:
	explicit Joydev(QObject *parent = 0);
	~Joydev(void);

	QStringList getJoydevs(void);
	bool isOpen(void);

    int getAxes(void);
    int getButtons(void);

	
signals:
	void axis1Update(int value);
	void axis2Update(int value);
	void axis3Update(int value);
	void axis4Update(int value);

	void buton1Event(bool isDown);
	void buton2Event(bool isDown);
	void buton3Event(bool isDown);
	void buton4Event(bool isDown);
	void buton5Event(bool isDown);
	void buton6Event(bool isDown);
	void buton7Event(bool isDown);
	void buton8Event(bool isDown);
	void buton9Event(bool isDown);
	void buton10Event(bool isDown);

	void opened(void);
	void closed(void);
	void devicesUpdated(void);
	void error(QString mssg);

	void startEventLoop(void);
	
public slots:
	void open(int id);
	void close(void);

private slots:
	void onTimerTick(void);
	void handleEvents(void);

private:
	void axisEvent(SDL_JoyAxisEvent event);
	void buttonEvent(SDL_JoyButtonEvent event);

};

#endif // JOYSTICK_H
