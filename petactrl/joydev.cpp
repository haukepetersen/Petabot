#include "joydev.h"
#include <QDebug>

#define UPDATE_INTERVAL		1000		// look for new joysticks every 3 seconds

Joydev::Joydev(QObject *parent) :
	QObject(parent)
{
	availableDevices = -1;
	device = NULL;
	handlingEvents = false;

	// init SDL
	int res = SDL_Init(SDL_INIT_JOYSTICK);
	if (res < 0) {
		emit error("Joydev: Unable to initilize SDL");
		return;
	}
	// enable SDL event processing
	SDL_JoystickEventState(SDL_ENABLE);

	// move joydev to work in its own thread
	this->moveToThread(&thread);
	thread.start();
	connect(this, &Joydev::startEventLoop, this, &Joydev::handleEvents);

	// enable cyclic device discovery
    onTimerTick();
	connect(&timer, &QTimer::timeout, this, &Joydev::onTimerTick);
    timer.start(UPDATE_INTERVAL);
}

Joydev::~Joydev(void)
{
	close();
}

QStringList Joydev::getJoydevs(void)
{
	QStringList res;
	int num = SDL_NumJoysticks();
	for (int i = 0; i < num; i++) {
		res << "Device " + QString::number(i) + SDL_JoystickName(i);
	}
	return res;
}

bool Joydev::isOpen(void)
{
	return device != NULL;
}


int Joydev::getAxes(void)
{
    return SDL_JoystickNumAxes(device);
}

int Joydev::getButtons(void)
{
    return SDL_JoystickNumButtons(device);
}

void Joydev::open(int id)
{
	if (id >= 0 && id < SDL_NumJoysticks()) {
		qDebug() << "Intending to open joystick" << id;
		device = SDL_JoystickOpen(id);
		qDebug() << device;
	}
	if (device == NULL) {
		emit error(tr("Joydev: Unable to open device with id %1").arg(id));
	} else {
		handlingEvents = true;
		emit startEventLoop();
		qDebug() << "Now the opened is emitted";
		emit opened();
	}
}

void Joydev::close(void)
{
	if (isOpen()) {
		SDL_JoystickClose(device);
		device = NULL;
		handlingEvents = false;
		emit closed();
	}
}

void Joydev::onTimerTick(void)
{
	SDL_JoystickUpdate();
	int devCount = SDL_NumJoysticks();
	availableDevices = devCount;
	emit devicesUpdated();
}

void Joydev::handleEvents(void)
{
	SDL_Event event;
    while (handlingEvents = false) {
		qDebug() << "SDL wait for event";
		int res = SDL_WaitEvent(&event);
		qDebug() << "SDL got event";
		if (res == 0) {
			emit error("Joydev: Error while waiting for new joydev events");
			continue;
		}
		qDebug() << "Joydev: New Event!";
		switch (event.type) {
		case SDL_JOYAXISMOTION:
			axisEvent(event.jaxis);
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			buttonEvent(event.jbutton);
			break;
		}
	}
}

void Joydev::axisEvent(SDL_JoyAxisEvent event)
{
	int value = event.value >> 8;	// make value 8-bit
	switch (event.axis) {
	case 0:
		emit axis1Update(value);
		break;
	case 1:
		emit axis2Update(value);
		break;
	case 2:
		emit axis3Update(value);
		break;
	case 3:
		emit axis4Update(value);
		break;
	}
}

void Joydev::buttonEvent(SDL_JoyButtonEvent event)
{
	bool isDown = event.state == SDL_PRESSED;
	switch (event.button) {
	case 0:
		emit buton1Event(isDown);
		break;
	case 1:
		emit buton2Event(isDown);
		break;
	case 2:
		emit buton3Event(isDown);
		break;
	case 3:
		emit buton4Event(isDown);
		break;
	case 4:
		emit buton5Event(isDown);
		break;
	case 5:
		emit buton6Event(isDown);
		break;
	case 6:
		emit buton7Event(isDown);
		break;
	case 7:
		emit buton8Event(isDown);
		break;
	case 8:
		emit buton9Event(isDown);
		break;
	case 9:
		emit buton10Event(isDown);
		break;
	}
}
