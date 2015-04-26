#include "testcommander.h"
#include "ui_testcommander.h"

#include <QDebug>

#include "commandparser.h"

TestCommander::TestCommander(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TestCommander)
{
	ui->setupUi(this);
	tcpSocket = new TcpSocket(this);
	connect(tcpSocket, &TcpSocket::received, this, &TestCommander::onReceive);
	connect(tcpSocket, &TcpSocket::connected, this, &TestCommander::onTcpOpened);
	connect(tcpSocket, &TcpSocket::disconnected, this, &TestCommander::onTcpClosed);
	connect(tcpSocket, &TcpSocket::error, this, &TestCommander::onTcpError);
	connect(this, &TestCommander::send, tcpSocket, &TcpSocket::send);

	uartSocket = new UartSocket(this);
	connect(uartSocket, &UartSocket::received, this, &TestCommander::onReceive);
	connect(uartSocket, &UartSocket::connected, this, &TestCommander::onUartOpened);
	connect(uartSocket, &UartSocket::disconnected, this, &TestCommander::onUartClosed);
	connect(uartSocket, &UartSocket::error, this, &TestCommander::onUartError);
	connect(uartSocket, &UartSocket::deviceDiscovered, this, &TestCommander::onUartDeviceUpdate);
	connect(uartSocket, &UartSocket::deviceRemoved, this, &TestCommander::onUartDeviceUpdate);
	connect(this, &TestCommander::send, uartSocket, &UartSocket::send);

	onUartDeviceUpdate();
	foreach (QString br, uartSocket->getBaudrates()) {
		ui->uartBaudrate->addItem(br);
		ui->uartBaudrate->setCurrentIndex(ui->uartBaudrate->count() - 1);
	}

	// for debugging purposes
	connect(this, &TestCommander::send, this, &TestCommander::onReceiveDebug);
}

TestCommander::~TestCommander()
{
	delete ui;
}


/**!
 * Private slots
 */

void TestCommander::onReceive(Command c)
{
	qDebug() << "Received something" << c.getType();
	if (c.isWrite()) {
		switch (c.getType()) {
		case CMD_MOTOR:

			break;
		case CMD_SERVO:

			break;
		case CMD_SWITCH:

			break;
		case CMD_STATUS:
			break;
		default:
			qDebug() << "Unknown message received";
		}
	}
}

void TestCommander::onReceiveDebug(Command c)
{
	qDebug() << "Received: " << CommandParser::toString(c);
}

void TestCommander::onTcpOpened(void)
{
	ui->tcpOpenClose->setEnabled(true);
	ui->tcpStatus->setText(tr("Tcp connection established to ") + tcpSocket->getHost() + tr(" on Port ") +
						   QString::number(tcpSocket->getPort()));
}

void TestCommander::onTcpClosed(void)
{
	ui->tcpStatus->setText(tr("Tcp is not connected"));
	tcpEnable();
}

void TestCommander::onTcpError(QString mssg)
{
	ui->tcpStatus->setText(tr("Tcp Error: ") + mssg);
	tcpEnable();
}

void TestCommander::onUartOpened(void)
{
	ui->uartBaudrate->setEnabled(false);
	ui->uartDevice->setEnabled(false);
	ui->uartOpenClose->setText(tr("Close"));
	ui->uartStatus->setText(tr("Uart connection established on port ") + uartSocket->getPort() +
							tr(" using a baudrate of ") + QString::number(uartSocket->getBaudrate()));
}

void TestCommander::onUartClosed(void)
{
	ui->uartBaudrate->setEnabled(true);
	ui->uartDevice->setEnabled(true);
	ui->uartOpenClose->setText(tr("Open"));
	ui->uartStatus->setText(tr("An Uart device is not opened"));
}

void TestCommander::onUartError(QString mssg)
{
	ui->uartStatus->setText(mssg);
}


void TestCommander::onUartDeviceUpdate(void)
{
	ui->uartDevice->clear();
	foreach (QString dev, uartSocket->getDevices()) {
		ui->uartDevice->addItem(dev);
	}
}

void TestCommander::on_tcpOpenClose_clicked(void)
{
	if (tcpSocket->isOpen()) {
		tcpSocket->close();
		qDebug() << "close tcp connection";
	} else {
		tcpSocket->open();
		tcpDisable();
		qDebug() << "open tcp connection";
	}
}

void TestCommander::on_uartOpenClose_clicked(void)
{
	if (uartSocket->isOpen()) {
		uartSocket->close();
	} else {
		uartSocket->setBaudrate(ui->uartBaudrate->currentIndex());
		uartSocket->setDevice(ui->uartDevice->currentText());
		qDebug() << "Uart with " << uartSocket->getPort() << " and " << uartSocket->getBaudrate();
		uartSocket->open();
	}
}

void TestCommander::on_read_0_clicked()
{
	qDebug() << "send_0_clicked()";
}

void TestCommander::on_read_1_clicked()
{
	emit send(CommandParser::parseMotorRead(ui->motChan->currentIndex()));
}

void TestCommander::on_read_2_clicked()
{
	emit send(CommandParser::parseServoRead(ui->servChan->currentIndex()));
}

void TestCommander::on_read_3_clicked()
{
	qDebug() << "send_3_clicked()";
}

void TestCommander::on_read_4_clicked()
{
	qDebug() << "send_4_clicked()";
}

void TestCommander::on_read_5_clicked()
{
	qDebug() << "send_5_clicked()";
}

void TestCommander::on_read_6_clicked()
{
	emit send(CommandParser::parseSwitchRead(ui->swChan->value()));
}

void TestCommander::on_read_7_clicked()
{
	qDebug() << "send_7_clicked()";
}

void TestCommander::on_read_8_clicked()
{
	qDebug() << "send_8_clicked()";
}

void TestCommander::on_read_9_clicked()
{
	qDebug() << "send_9_clicked()";
}

void TestCommander::on_read_10_clicked()
{
	qDebug() << "send_10_clicked()";
}

void TestCommander::on_read_11_clicked()
{
	qDebug() << "send_11_clicked()";
}

void TestCommander::on_read_12_clicked()
{
	qDebug() << "send_12_clicked()";
}

void TestCommander::on_read_13_clicked()
{
	qDebug() << "send_13_clicked()";
}

void TestCommander::on_read_14_clicked()
{
	qDebug() << "send_14_clicked()";
}

void TestCommander::on_read_15_clicked()
{
	emit send(CommandParser::parseStatusRead());
}



void TestCommander::on_write_0_clicked()
{
	qDebug() << "write_0_clicked()";
}

void TestCommander::on_write_1_clicked()
{
	int chan = ui->motChan->currentIndex();
	int mode = ui->motMode->currentIndex();
	int value = ui->motVal->value();
	emit send(CommandParser::parseMotorWrite(chan, mode, value));
}

void TestCommander::on_write_2_clicked()
{
	int chan = ui->servChan->currentIndex();
	int mode = ui->servMode->currentIndex();
	int value = ui->servVal->value();
	emit send(CommandParser::parseServoWrite(chan, mode, value));
}

void TestCommander::on_write_3_clicked()
{
	qDebug() << "write_3_clicked()";
}

void TestCommander::on_write_4_clicked()
{
	qDebug() << "write_4_clicked()";
}

void TestCommander::on_write_5_clicked()
{
	qDebug() << "write_5_clicked()";
}

void TestCommander::on_write_6_clicked()
{
	int chan = ui->swChan->value();
	bool on = ui->swOn->isChecked();
	emit send(CommandParser::parseSwitchWrite(chan, on));
}

void TestCommander::on_write_7_clicked()
{
	qDebug() << "write_7_clicked()";
}

void TestCommander::on_write_8_clicked()
{
	qDebug() << "write_8_clicked()";
}

void TestCommander::on_write_9_clicked()
{
	qDebug() << "write_9_clicked()";
}

void TestCommander::on_write_10_clicked()
{
	qDebug() << "write_10_clicked()";
}

void TestCommander::on_write_11_clicked()
{
	qDebug() << "write_11_clicked()";
}

void TestCommander::on_write_12_clicked()
{
	qDebug() << "write_12_clicked()";
}

void TestCommander::on_write_13_clicked()
{
	qDebug() << "write_13_clicked()";
}

void TestCommander::on_write_14_clicked()
{
	qDebug() << "write_14_clicked()";
}

void TestCommander::on_write_15_clicked()
{
	emit send(CommandParser::parseStatusWrite(0xff));
}

void TestCommander::tcpEnable()
{
	ui->tcpPort->setEnabled(true);
	ui->tcpHost->setEnabled(true);
	ui->tcpOpenClose->setEnabled(true);
	ui->tcpOpenClose->setText(tr("Open"));
}

void TestCommander::tcpDisable()
{
	ui->tcpPort->setEnabled(false);
	ui->tcpHost->setEnabled(false);
	ui->tcpOpenClose->setEnabled(false);
	ui->tcpOpenClose->setText(tr("Close"));
}
