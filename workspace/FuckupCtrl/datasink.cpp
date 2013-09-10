#include "datasink.h"
#include "ui_datasink.h"
#include "QDebug"
#include <qextserialenumerator.h>

DataSink::DataSink(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DataSink)
{
	ui->setupUi(this);
	on_uartDeviceRefresh_clicked();

	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_newConnection()));
	tcpConnection = NULL;
	tcpIsStarted = false;

	PortSettings uartSettings;
	uartSettings.DataBits = DATA_8;
	uartSettings.FlowControl = FLOW_OFF;
	uartSettings.Parity = PAR_NONE;
	uartSettings.StopBits = STOP_1;
	uartSettings.Timeout_Millisec = 1000;
	uart = new QextSerialPort(uartSettings, QextSerialPort::EventDriven, this);
}

DataSink::~DataSink()
{
	delete ui;
	delete tcpServer;
	delete uart;
}

void DataSink::closeEvent(QCloseEvent *e)
{
	// put socket and uart disconnecting stuff here
}

void DataSink::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == 16777221 || e->key() == 16777220) {
		if (ui->tcpText->hasFocus() && ui->tcpSend->isEnabled()) {
			on_tcpSend_clicked();
		} else if (ui->uartText->hasFocus() && ui->uartSend->isEnabled()) {
			on_uartSend_clicked();
		}
	}
}

void DataSink::tcp_print(QString text)
{
	ui->tcpConsole->insertPlainText(text + "\n");
}

void DataSink::uart_print(QString text)
{
	ui->uartConsole->insertPlainText(text + "\n");
}

void DataSink::on_closeWindow_clicked(void)
{
	this->close();
}

void DataSink::on_tcpStartStop_clicked(void)
{
	if (tcpIsStarted) {
		if (tcpConnection != NULL) {
			tcpConnection->close();
			tcpConnection = NULL;
		}
		tcpServer->close();
		tcpIsStarted = false;
		ui->tcpStatus->setText(tr("Stopped"));
		ui->tcpStartStop->setText(tr("Start Server"));
	} else {
		int port = ui->tcpPort->text().toInt();
		if (tcpServer->listen(QHostAddress::Any, port)) {
			tcpIsStarted = true;
			ui->tcpStatus->setText(tr("Listening"));
			ui->tcpStartStop->setText(tr("Stop Server"));
		}
	}
}

void DataSink::on_uartOpenClose_clicked(void)
{
	if (uart->isOpen()) {
		uart->close();
		ui->uartOpenClose->setText(tr("Open"));
		ui->uartSend->setEnabled(false);
		ui->uartStatus->setText(tr("Closed"));
	} else {
		if (ui->uartDevice->count() < 1) {
			uart_print(tr("Unable to open UART, no port present."));
			return;
		}
		QString port = ui->uartDevice->itemText(ui->uartDevice->currentIndex());
		uart->setPortName(port);
		switch(ui->uartBaudrate->currentIndex())
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
			uart->setBaudRate(BAUD19200);
			break;
        case 7:
			uart->setBaudRate(BAUD38400);
			break;
        case 8:
			uart->setBaudRate(BAUD57600);
			break;
        case 9:
			uart->setBaudRate(BAUD115200);
			break;
		}
		if (uart->open(QIODevice::ReadWrite)) {
			ui->uartOpenClose->setText(tr("Close"));
			ui->uartSend->setEnabled(true);
			ui->uartStatus->setText(uart->portName() + " " + tr("open"));
		} else {
			uart_print("Unable to open uart port: " + uart->portName());
		}
	}
}

void DataSink::on_tcpSend_clicked(void)
{
	if (tcpConnection != NULL && ui->tcpSend->isEnabled()) {
		QByteArray data = ui->tcpText->text().toLocal8Bit();
		tcpConnection->write(data.data(), data.length());
		tcp_print("SEND: " + ui->tcpText->text());
		ui->tcpText->clear();
	}
}

void DataSink::on_uartSend_clicked(void)
{
	if (uart->isOpen() && ui->uartSend->isEnabled()) {
		QByteArray data = ui->uartText->text().toLocal8Bit();
		uart->write(data.data(), data.length());
		uart_print("SEND: " + ui->uartText->text());
		ui->uartText->clear();
	}
}

void DataSink::on_uartDeviceRefresh_clicked(void)
{
	// remove all items
	while (ui->uartDevice->count() > 0) {
		ui->uartDevice->removeItem(0);
	}
	// get available devices
	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	foreach (QextPortInfo info, ports) {
		ui->uartDevice->addItem(info.portName);
	}
}

void DataSink::tcp_newConnection(void)
{
	qDebug() << "New Connection Request";
	if (tcpConnection == NULL) {	// accept connection
		tcpConnection = tcpServer->nextPendingConnection();
//		tcpServer->pauseAccepting();
		connect(tcpConnection, SIGNAL(disconnected()), this, SLOT(tcp_disconnected()));
		connect(tcpConnection, SIGNAL(readyRead()), this, SLOT(tcp_received()));
		ui->tcpSend->setEnabled(true);
		ui->tcpStatus->setText(tr("Connected"));
    } else {
        QTcpSocket *tmp = tcpServer->nextPendingConnection();
        tmp->close();
    }
}

void DataSink::tcp_disconnected(void)
{
	tcpConnection->close();
	tcpConnection = NULL;
//	tcpServer->resumeAccepting();
	ui->tcpSend->setEnabled(false);
	ui->tcpStatus->setText(tr("Listening"));
}

void DataSink::tcp_received(void)
{
	char buf[1024];
	while (tcpConnection->bytesAvailable() > 0) {
		int n = tcpConnection->read(buf, 1023);
		buf[n] = '\0';
		tcp_print("REC:  " + QString::fromLocal8Bit(buf) + " [" + QString::number(n) + "]");
	}
}

void DataSink::uart_received(void)
{
	char buf[1024];
	while (uart->bytesAvailable() > 0) {
		int n = uart->read(buf, 1023);
		buf[n] = '\0';
		uart_print("REC:  " + QString::fromLocal8Bit(buf) + " [" + QString::number(n) + "]");
	}
}
