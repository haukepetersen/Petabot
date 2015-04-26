#include "tcpsocket.h"

#include "QHostAddress"

#define DEFAULT_PORT	23500
#define DEFAULT_HOST	"127.0.0.1"

TcpSocket::TcpSocket(QObject *parent) :
	QObject(parent)
{
	port = DEFAULT_PORT;
	host = DEFAULT_HOST;
	openStatus = false;
	socket = new QTcpSocket(this);
	connectSocket();

	// connect the parser
	connect(&parser, &CommandParser::commandReady, this, &TcpSocket::onNewCommand);
}

void TcpSocket::setSocket(QTcpSocket *newSocket)
{
	// disconnect and dispose old socket
	socket->close();
	delete socket;
	socket = newSocket;
	// reconnect the new socket
	connectSocket();
	openStatus = true;
}

bool TcpSocket::isOpen(void)
{
	return openStatus;
}

int TcpSocket::getPort(void)
{
	return port;
}

QString TcpSocket::getHost()
{
	return host;
}

void TcpSocket::connectSocket(void)
{
	// connect the socket properly
	connect(socket, &QTcpSocket::connected, this, &TcpSocket::onOpened);
	connect(socket, &QTcpSocket::disconnected, this, &TcpSocket::onClosed);
	connect(socket, &QTcpSocket::readyRead, this, &TcpSocket::onNewData);
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void TcpSocket::open(void)
{
	if (openStatus) {
		close();
	} else {
		socket->connectToHost(QHostAddress(host), port);
	}
}

void TcpSocket::close(void)
{
	socket->close();
}

void TcpSocket::send(Command c)
{
	qDebug() << "Command out" << CommandParser::toString(c);
	if (openStatus) {
		socket->write(c.rawData(), c.length());
	}
}

void TcpSocket::setHost(QString host)
{
	this->host = host;
}

void TcpSocket::setPort(int port)
{
	this->port = port;
}



void TcpSocket::onNewData(void)
{
	parser.onNewData(socket->readAll());
}

void TcpSocket::onNewCommand(Command c)
{
	emit received(c);
}

void TcpSocket::onClosed(void)
{
	openStatus = false;
	emit disconnected();
}

void TcpSocket::onOpened(void)
{
	openStatus = true;
	emit connected();
}

void TcpSocket::onError(QAbstractSocket::SocketError e)
{
	QString mssg =  e + ": " + socket->errorString();
	emit error(mssg);
}
