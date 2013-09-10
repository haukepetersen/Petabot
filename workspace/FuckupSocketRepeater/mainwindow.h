#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "qextserialport.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT


private:
	Ui::MainWindow *ui;
	bool connected;
	QTcpServer *server;
	QTcpSocket *client;
	QextSerialPort *uart;
	PortSettings uartSettings;

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void startstop(void);
	void newConnection(void);
	void incomingData(void);
	void openUartPort(void);
	void uartNewData(void);
	void onTcpClose(void);

public slots:
	void print(QString txt);
	void refreshUartPorts(void);

};

#endif // MAINWINDOW_H
