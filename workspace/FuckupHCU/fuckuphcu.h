#ifndef FUCKUPHCU_H
#define FUCKUPHCU_H

#include <QObject>
#include <QTcpServer>
#include <QString>
#include "tcpsocket.h"
#include "uartsocket.h"

// define the standard values for port, serial device and baudrate
#define DEFAULT_PORT	23500
#define DEFAULT_DEV		"/dev/ttyAMA0"
#define DEFAULT_BAUD	9

class FuckupHCU : public QObject
{
	Q_OBJECT

private:
	TcpSocket *tcpChannel;
	QTcpServer *tcpServer;
	UartSocket *uartChannel;
	// connection information
	int tcpPort;
	QString uartPort;
	int uartBaud;
	bool connected;

public:
	explicit FuckupHCU(QObject *parent = 0);
	~FuckupHCU(void);

	bool start(void);
	
signals:
	
public slots:
	void tcpListenIn(Command c);
	void uartListenIn(Command c);
	void exit(void);

private slots:
	void onTcpNewConnection(void);
	void onTcpDisconnected(void);

private:
	bool parseArgs(QStringList args);
	
};

#endif // FUCKUPHCU_H
