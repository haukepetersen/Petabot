#include "uartsocket.h"

#include "QRegExp"
#include "QIODevice"
#include "qextserialenumerator.h"

#include <QDebug>

UartSocket::UartSocket(QObject *parent) :
    QObject(parent)
{
    // define generic uart port config - leave port name and baudrate open
    PortSettings portconf;
    portconf.BaudRate = BAUD115200;
    portconf.DataBits = DATA_8;
    portconf.FlowControl = FLOW_OFF;
    portconf.Parity = PAR_NONE;
    portconf.StopBits = STOP_1;

    uart = new QextSerialPort(portconf, QextSerialPort::EventDriven, this);
    connect(uart, SIGNAL(readyRead()), this, SLOT(onNewData()));

	pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(onPollEvent()));
	pollTimer->start(3000);		// look every 3 seconds for a device change
	discoverdPorts = portenum.getPorts().size();

	// connect the parser
    connect(&parser, SIGNAL(commandReady(Command)), this, SLOT(onNewCommand(Command)));
}

UartSocket::~UartSocket(void)
{
    delete uart;            // calls implicitly uart.close()?!
}

QStringList UartSocket::getDevices(void)
{
    QStringList res;
    QRegExp regexNoPort("tty");
    foreach (QextPortInfo port, portenum.getPorts()) {
        if (regexNoPort.indexIn(port.portName) < 0) {
            res.append(port.portName);
        }
    }
    return res;
}

QStringList UartSocket::getBaudrates(void)
{
    QStringList res;
    res << "300" << "600" << "1200" << "2400" << "4800" << "9600";
    res << "19200" << "38400" << "57600" << "115200";
    return res;
}

bool UartSocket::isOpen(void)
{
    return uart->isOpen();
}

QString UartSocket::getPort(void)
{
	return port;
}

int UartSocket::getBaudrate(void)
{
	return baudrate;
}

void UartSocket::open(void)
{
    if (uart->isOpen()) {
		close();
    }
	if (uart->open(QIODevice::ReadWrite)) {
        emit connected();
	} else {
		emit error(tr("Unable to open port ") + uart->portName());
	}
}

void UartSocket::close(void)
{
    uart->close();
    onClose();
}

void UartSocket::send(Command c)
{
	if (uart->isOpen()) {
		int res = uart->write(c.rawData(), c.length());
		if (res < 0) {
			emit error(QString(tr("Write on Port %1 failed, closing port")).arg(uart->portName()));
			uart->close();
		}
    }
}

void UartSocket::setDevice(QString device)
{
    uart->setPortName(device);
	port = device;
}

void UartSocket::setBaudrate(int index)
{
    switch(index)
    {
    case 0:
        uart->setBaudRate(BAUD300);
		baudrate = 300;
        break;
    case 1:
        uart->setBaudRate(BAUD600);
		baudrate = 600;
        break;
    case 2:
        uart->setBaudRate(BAUD1200);
		baudrate = 1200;
        break;
    case 3:
        uart->setBaudRate(BAUD2400);
		baudrate = 2400;
        break;
    case 4:
        uart->setBaudRate(BAUD4800);
		baudrate = 4800;
        break;
    case 5:
        uart->setBaudRate(BAUD9600);
		baudrate = 9600;
        break;
    case 6:
        uart->setBaudRate(BAUD19200);
		baudrate = 19200;
        break;
    case 7:
        uart->setBaudRate(BAUD38400);
		baudrate = 38400;
        break;
    case 8:
        uart->setBaudRate(BAUD57600);
		baudrate = 57600;
        break;
    case 9:
    default:
        uart->setBaudRate(BAUD115200);
		baudrate = 115200;
        break;
    }
}

void UartSocket::onPollEvent(void)
{
	int tmp = portenum.getPorts().size();
	if (tmp > discoverdPorts) {
		emit deviceDiscovered();
	} else if (tmp < discoverdPorts) {
		emit deviceRemoved();
	}
	discoverdPorts = tmp;
}

void UartSocket::onNewData(void)
{
    parser.onNewData(uart->readAll());
}

void UartSocket::onNewCommand(Command c)
{
    emit received(c);
}

void UartSocket::onClose()
{
    emit disconnected();
}
