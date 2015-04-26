#ifndef UARTSOCKET_H
#define UARTSOCKET_H

#include <QObject>
#include <QStringList>
#include <qextserialport.h>
#include <qextserialenumerator.h>
#include <QTimer>

#include "command.h"
#include "commandparser.h"

/*!
 * \brief The UartSocket class
 * \author Hauke Petersen
 */
class UartSocket : public QObject
{
    Q_OBJECT

private:
    QextSerialEnumerator portenum;
	int discoverdPorts;
	QTimer *pollTimer;
    QextSerialPort *uart;
    CommandParser parser;
	QString port;
	int baudrate;

public:
    explicit UartSocket(QObject *parent = 0);
    ~UartSocket(void);

    QStringList getDevices(void);
    QStringList getBaudrates(void);

	bool isOpen(void);

	QString getPort(void);
	int getBaudrate(void);
    
signals:
    void received(Command c);
    void connected(void);
    void disconnected(void);
	void error(QString mssg);

    void deviceDiscovered();
    void deviceRemoved();
    
public slots:
    void open(void);
    void close(void);
    void send(Command c);
    void setDevice(QString device);
    void setBaudrate(int index);
	void onPollEvent(void);

private slots:
    void onNewData(void);
    void onNewCommand(Command c);
    void onClose();
};

#endif // UARTSOCKET_H
