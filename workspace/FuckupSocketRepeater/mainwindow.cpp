#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialenumerator.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	connected = false;
	server = new QTcpServer(parent);
	client = NULL;
	uartSettings.DataBits = DATA_8;
	uartSettings.FlowControl = FLOW_OFF;
	uartSettings.Parity = PAR_NONE;
	uartSettings.StopBits = STOP_1;
	uartSettings.Timeout_Millisec = 1000;
	uart = new QextSerialPort(uartSettings, QextSerialPort::EventDriven, this);
	ui->setupUi(this);
	connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startstop()));
	connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	connect(ui->uartRefresh, SIGNAL(clicked()), this, SLOT(refreshUartPorts()));
	connect(ui->uartOpen, SIGNAL(clicked()), this, SLOT(openUartPort()));
	connect(uart, SIGNAL(readyRead()), this, SLOT(uartNewData()));
	refreshUartPorts();
	print("foo");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::print(QString txt)
{
	ui->txtOut->insertPlainText(txt + "\n");
}

void MainWindow::startstop(void)
{
	if (connected) {
		print ("Stopping Server");
		if (client != NULL) {
			client->close();
			client = NULL;
		}
		server->close();
		if (server->isListening() == false) {
			print("Server was stopped");
		} else {
			print("Unable to stop server");
		}
		ui->btnStart->setText("Start");
	} else {
		int port = ui->port->text().toInt();
		if (port <= 0) {
			print("Unable to start server, no valid port given");
			return;
		}
		print("Starting server at port " + QString::number(port));
		if (server->listen(QHostAddress::Any, port)) {
			print("Server was started on port " + QString::number(port));
		} else {
			print("Unable to start server");
		}
		ui->btnStart->setText("Stop");
	}
	connected = !connected;
}

void MainWindow::newConnection(void)
{
	if (client != NULL) {
		print("New connection dropped, already a connection present");
	} else {
		server->pauseAccepting();
		client = server->nextPendingConnection();
		connect(client, SIGNAL(readyRead()), this, SLOT(incomingData()));
		connect(client, SIGNAL(disconnected()), this, SLOT(onTcpClose()));
		print("New connection accepted from " + client->peerName() + "  " + client->peerAddress().toString());
	}
}

void MainWindow::onTcpClose(void)
{
	client->close();
	client = NULL;
	print("Connection was closed");
}

void MainWindow::incomingData(void)
{
	char tmp[1024];
	while (client->bytesAvailable() > 0) {
		int n = client->read(tmp, 1023);
		tmp[n] = '\0';
		print("DATA: " + QString(tmp) + "  [" + QString::number(n) + "bytes]");
	}
}

void MainWindow::refreshUartPorts(void)
{
	// remove all items
	while (ui->uartPort->count() > 0) {
		ui->uartPort->removeItem(0);
	}
	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	foreach (QextPortInfo info, ports) {
		ui->uartPort->addItem(info.portName);
	}
}

void MainWindow::openUartPort(void)
{
	if (uart->isOpen()) {
		uart->close();
		ui->uartOpen->setText("Open");
		print("Closed connection to port " + uart->portName());
	} else {
		if (ui->uartPort->count() <= 0) {
			print("Unable to open UART, no port present.");
			return;
		}
		QString port = ui->uartPort->itemText(ui->uartPort->currentIndex());
		print("Connecting to port: " + port);
		uart->setPortName(port);
		switch(ui->uartBaud->currentIndex())
		{
		case 0:
			uart->setBaudRate(BAUD300);
			break;
		case 1:
			uart->setBaudRate(BAUD600);
			break;
		case 2:
			uart->setBaudRate(BAUD1200);
			break;
		case 3:
			uart->setBaudRate(BAUD2400);
			break;
		case 4:
			uart->setBaudRate(BAUD4800);
			break;
		case 5:
			uart->setBaudRate(BAUD9600);
			break;
		case 6:
			uart->setBaudRate(BAUD14400);
			break;
		case 7:
			uart->setBaudRate(BAUD19200);
			break;
		case 8:
			uart->setBaudRate(BAUD38400);
			break;
		case 9:
			uart->setBaudRate(BAUD57600);
			break;
		case 10:
			uart->setBaudRate(BAUD115200);
			break;
		}
		if (uart->open(QIODevice::ReadWrite)) {
			ui->uartOpen->setText("Close");
			print("Uart is now connected to port: " + uart->portName());
		} else {
			print("Unable to open uart port: " + uart->portName());
		}
	}
}

void MainWindow::uartNewData(void)
{
	char buf[1024];
	while (uart->bytesAvailable() > 0) {
		int n = uart->read(buf, 1023);
		buf[n] = '\0';
		print("UART DATA: " + QString::fromLocal8Bit(buf) + "[" + QString::number(n) + "]");
	}
}
