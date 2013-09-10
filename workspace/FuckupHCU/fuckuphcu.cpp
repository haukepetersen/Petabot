#include "fuckuphcu.h"
#include <QCoreApplication>
#include <QStringList>
#include <QRegExp>
#include <QString>

#include "comm.h"

#include <QDebug>



FuckupHCU::FuckupHCU(QObject *parent) :
	QObject(parent)
{
	tcpPort = DEFAULT_PORT;
	uartPort = DEFAULT_DEV;
	uartBaud = DEFAULT_BAUD;
	connected = false;

	tcpChannel = new TcpSocket(this);
	uartChannel = new UartSocket(this);
	tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onTcpNewConnection()));
    connect(tcpChannel, SIGNAL(disconnected()), this, SLOT(onTcpDisconnected()));

	// connect tcp channel to uart channel and reverse
    connect(tcpChannel, SIGNAL(received(Command)), uartChannel, SLOT(send(Command)));
    connect(tcpChannel, SIGNAL(received(Command)), this, SLOT(tcpListenIn(Command)));
    connect(uartChannel, SIGNAL(received(Command)), tcpChannel, SLOT(send(Command)));
    connect(uartChannel, SIGNAL(received(Command)), this, SLOT(uartListenIn(Command)));
}

FuckupHCU::~FuckupHCU(void)
{
	delete tcpChannel;
	delete tcpServer;
	delete uartChannel;
}


bool FuckupHCU::start(void)
{
	QStringList args = QCoreApplication::arguments();
	if (parseArgs(args)) {
		uartChannel->setDevice(uartPort);
		uartChannel->setBaudrate(uartBaud);
		uartChannel->open();
		if (uartChannel->isOpen()) {
			qDebug() << "UART CHANNEL up";
		} else {
			qDebug() << "UART CHANNEL can not be opened";
			return false;
		}
		if (tcpServer->listen(QHostAddress::Any, tcpPort)) {
			qDebug() << "TCP SERVER up";
		} else {
			qDebug() << "TCP SERVER can not start";
			return false;
		}
	} else {
		return false;
	}
	return true;
}

void FuckupHCU::tcpListenIn(Command c)
{
	qDebug() << "TCP CHANNEL  received" << CommandParser::toString(c);
	if (c.getType() == COMM_STAT_CMD) {
		Command c = CommandParser::parseStatusWrite(0xf8);
		tcpChannel->send(c);
	}
}

void FuckupHCU::uartListenIn(Command c)
{
	qDebug() << "UART CHANNEL received" << CommandParser::toString(c);
	if (c.getType() == COMM_STAT_CMD) {
		Command c = CommandParser::parseStatusWrite(0xf8);
		uartChannel->send(c);
	}
}

void FuckupHCU::exit(void)
{
	QCoreApplication::exit(0);
}

void FuckupHCU::onTcpNewConnection(void)
{
	QTcpSocket *tmp = tcpServer->nextPendingConnection();
	if (!connected) {
		tcpChannel->setSocket(tmp);
		connected = true;
		qDebug() << "TCP SERVER accepted connection from" << tmp->peerAddress().toString();
	} else {
		qDebug() << "TCP SERVER rejected connection from" << tmp->peerAddress().toString();
		tmp->close();
	}
}

void FuckupHCU::onTcpDisconnected(void)
{
	qDebug() << "TCP CHANNEL was closed";
	connected = false;
}

bool FuckupHCU::parseArgs(QStringList args)
{
	QRegExp regexPort("port=(\\d+)");
    QRegExp regexDev("(device|dev)=([\a-zA-Z0-9]+)");
	QRegExp regexHelp("help");

	foreach (QString s, args) {
		if (regexHelp.indexIn(s) > -1) {
			qDebug() << "USAGE: fuckuphcu [dev=DEVICE] [port=PORT] [help]";
			return false;
		} else if (regexPort.indexIn(s) > -1) {
			tcpPort = regexPort.cap(1).toInt();
		} else if (regexDev.indexIn(s) > -1) {
            uartPort = regexDev.cap(2);
		}
	}
	qDebug();
	qDebug() << "Done parsing arguments, will run with the following configuration:";
	qDebug() << "   TCP SERVER PORT:" << tcpPort;
	qDebug() << "   UART DEVICE:    " << uartPort;
	qDebug() << "   UART BAUDRATE:  " << uartBaud;
	qDebug();
	return true;
}

