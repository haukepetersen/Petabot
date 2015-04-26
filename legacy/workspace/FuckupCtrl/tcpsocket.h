#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "command.h"
#include "commandparser.h"

class TcpSocket : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *socket;
	CommandParser parser;
	int port;
	QString host;
	bool openStatus;

public:
	explicit TcpSocket(QObject *parent = 0);

	void setSocket(QTcpSocket *newSocket);

	bool isOpen(void);
	int getPort(void);
	QString getHost(void);

private:
	void connectSocket(void);
    
signals:
	void received(Command c);
	void connected(void);
	void disconnected(void);
	void error(QString mssg);
    
public slots:
	void open(void);
	void close(void);
	void send(Command c);
	void setHost(QString host);
	void setPort(int port);

private slots:
	void onNewData(void);
	void onNewCommand(Command c);
	void onClosed(void);
	void onOpened(void);
	void onError(QAbstractSocket::SocketError e);
};

#endif // TCPSOCKET_H
