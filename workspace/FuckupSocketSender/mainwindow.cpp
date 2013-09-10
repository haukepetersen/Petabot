#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->connect, SIGNAL(clicked()), this, SLOT(connectToServer()));
	connect(ui->send, SIGNAL(clicked()), this, SLOT(sendData()));
	socket = new QTcpSocket();
	connected = false;
	connect(socket, SIGNAL(connected()), this, SLOT(onConnect()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(receivedData()));
	print("Sender is running now.");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::connectToServer(void)
{
	if (connected) {
		socket->close();
		print("Closed socket");
	} else {
		QHostAddress address(ui->host->text());
		int port = ui->port->text().toInt();
		if (port <= 0) {
			print ("unable to connect to port " + QString::number(port));
			return;
		}
		if (address.isNull()) {
			print ("unable to connect to host " + ui->host->text());
			return;
		}
		socket->connectToHost(address, port);
	}
}

void MainWindow::sendData(void)
{
	if (connected) {
		print ("send something" + ui->data->text());
		QByteArray data = ui->data->text().toLocal8Bit();
		socket->write(data.data(), data.length());
		ui->data->setText("");
	} else {
		print("not connected: not sending: " + ui->data->text());
	}
}

void MainWindow::receivedData(void)
{
	char buf[1024];
	while (socket->bytesAvailable() > 0) {
		int n = socket->read(buf, 1023);
		buf[n] = '\0';
		print("REC:  " + QString::fromLocal8Bit(buf) + " [" + QString::number(n) + "]");
	}

}

void MainWindow::print(QString txt)
{
	ui->txtOut->appendPlainText(txt);
}

void MainWindow::onConnect(void)
{
	connected = true;
	print("Connection was established.");
	ui->connect->setText("Disconnect");
}

void MainWindow::onDisconnect(void)
{
	connected = false;
	print("Connection was disconnected");
	ui->connect->setText("Connect");
}

void MainWindow::onError(QAbstractSocket::SocketError error)
{
	print("Error on socket: " + QString::number(error));
	ui->connect->setText("Connect");
}
