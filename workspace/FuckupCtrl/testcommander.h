#ifndef TESTCOMMANDER_H
#define TESTCOMMANDER_H

#include <QWidget>

#include "command.h"
#include "comm.h"
#include "tcpsocket.h"
#include "uartsocket.h"

namespace Ui {
class TestCommander;
}

class TestCommander : public QWidget
{
	Q_OBJECT

private:
	TcpSocket *tcpSocket;
	UartSocket *uartSocket;
	Ui::TestCommander *ui;
	
public:
	explicit TestCommander(QWidget *parent = 0);
	~TestCommander();

signals:
	void send(Command c);

private slots:
	void onReceive(Command c);

	void onReceiveDebug(Command c);

	void onTcpOpened(void);
	void onTcpClosed(void);
	void onTcpError(QString mssg);

	void onUartOpened(void);
	void onUartClosed(void);
	void onUartError(QString mssg);
	void onUartDeviceUpdate(void);

	void on_tcpOpenClose_clicked(void);
	void on_uartOpenClose_clicked(void);

	void on_read_0_clicked(void);
	void on_read_1_clicked(void);
	void on_read_2_clicked(void);
	void on_read_3_clicked(void);
	void on_read_4_clicked(void);
	void on_read_5_clicked(void);
	void on_read_6_clicked(void);
	void on_read_7_clicked(void);
	void on_read_8_clicked(void);
	void on_read_9_clicked(void);
	void on_read_10_clicked(void);
	void on_read_11_clicked(void);
	void on_read_12_clicked(void);
	void on_read_13_clicked(void);
	void on_read_14_clicked(void);
	void on_read_15_clicked(void);

	void on_write_0_clicked(void);
	void on_write_1_clicked(void);
	void on_write_2_clicked(void);
	void on_write_3_clicked(void);
	void on_write_4_clicked(void);
	void on_write_5_clicked(void);
	void on_write_6_clicked(void);
	void on_write_7_clicked(void);
	void on_write_8_clicked(void);
	void on_write_9_clicked(void);
	void on_write_10_clicked(void);
	void on_write_11_clicked(void);
	void on_write_12_clicked(void);
	void on_write_13_clicked(void);
	void on_write_14_clicked(void);
	void on_write_15_clicked(void);

private:
	void tcpDisable(void);
	void tcpEnable(void);
};

#endif // TESTCOMMANDER_H
