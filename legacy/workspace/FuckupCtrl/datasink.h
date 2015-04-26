#ifndef DATASINK_H
#define DATASINK_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <qextserialport.h>

namespace Ui {
class DataSink;
}

class DataSink : public QWidget
{
	Q_OBJECT

private:
	Ui::DataSink *ui;
	QTcpServer *tcpServer;
	QTcpSocket *tcpConnection;
	QextSerialPort *uart;
	bool tcpIsStarted;
	
public:
	explicit DataSink(QWidget *parent = 0);
	~DataSink();

private:
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);

	void uart_print(QString text);
	void tcp_print(QString text);

private slots:
	// handle buttons
	void on_closeWindow_clicked(void);
	void on_tcpStartStop_clicked(void);
	void on_tcpSend_clicked(void);
	void on_uartOpenClose_clicked(void);
	void on_uartSend_clicked(void);
	void on_uartDeviceRefresh_clicked(void);

	void tcp_newConnection(void);
	void tcp_disconnected(void);
	void tcp_received(void);

	void uart_received(void);
};

#endif // DATASINK_H
