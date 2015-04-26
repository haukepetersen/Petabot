#include "fuckupctrl.h"
#include "ui_fuckupctrl.h"
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>

#define SPEED_MAX	255
#define SPEED_MIN	-255
#define SPEED_STOP	0
#define STEER_L		-255
#define STEER_R		255
#define STEER_C		0

FuckupCtrl::FuckupCtrl(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::FuckupCtrl),
    datasinkWidget(NULL),
    inputConfigWidget(NULL)
{
	speed = 0;
	steer = 0;
	for (int i = 0; i < 8; i++) {
		switches[i] = false;
	}

	// set up communication channels
	tcpChannel = new TcpSocket(this);
	connect(tcpChannel, &TcpSocket::received, this, &FuckupCtrl::onDataReceive);
	connect(tcpChannel, &TcpSocket::connected, this, &FuckupCtrl::onTcpConnected);
	connect(tcpChannel, &TcpSocket::disconnected, this, &FuckupCtrl::onTcpDisconnected);
	connect(tcpChannel, &TcpSocket::error, this, &FuckupCtrl::onTcpError);
	connectTcp();

	uartChannel = new UartSocket(this);
	connect(uartChannel, &UartSocket::received, this, &FuckupCtrl::onDataReceive);
	connect(uartChannel, &UartSocket::connected, this, &FuckupCtrl::onUartConnected);
	connect(uartChannel, &UartSocket::disconnected, this, &FuckupCtrl::onUartDisconnected);
	connect(uartChannel, &UartSocket::error, this, &FuckupCtrl::onUartError);
	connect(uartChannel, &UartSocket::deviceDiscovered, this, &FuckupCtrl::onUartDeviceChanged);
	connect(uartChannel, &UartSocket::deviceRemoved, this, &FuckupCtrl::onUartDeviceChanged);

	ui->setupUi(this);
	this->grabKeyboard();

	// populate uart devices and baudrates
	ui->uartBaudrate->clear();
	foreach (QString s, uartChannel->getBaudrates()) {
		ui->uartBaudrate->addItem(s);
	}
	ui->uartBaudrate->setCurrentIndex(ui->uartBaudrate->count() - 1);
	onUartDeviceChanged();

	connect(ui->speed, &QSlider::valueChanged, this, &FuckupCtrl::onSpeedChange);
	connect(ui->steer, &QSlider::valueChanged, this, &FuckupCtrl::onSteerChange);

	// setup the joystick reader
	connect(&joydev, &Joydev::devicesUpdated, this, &FuckupCtrl::onJoydevUpdate);
	connect(&joydev, &Joydev::opened, this, &FuckupCtrl::onJoydevOpened);
	connect(&joydev, &Joydev::buton1Event, this, &FuckupCtrl::onButton1Press);

	//this->testPlot();
	ui->brake->setStyleSheet("* {background-color: red; border-radius: 7px; border-width: 1px; border-style: solid; border-color: white;}");
}

FuckupCtrl::~FuckupCtrl()
{
    delete ui;
}

void FuckupCtrl::onSpeedChange(int speed)
{
	if (this->speed != speed) {
		this->speed = speed;
		ui->speed->setValue(speed);
		Command c = CommandParser::parseMotorWrite(0, speed);
		// send command to connected comm-channels
		emit speedChanged(c);
	}
}

void FuckupCtrl::onSteerChange(int dir)
{
	if (this->steer != dir) {
		this->steer = dir;
		ui->steer->setValue(dir);
		Command c = CommandParser::parseServoWrite(0, dir);
		emit steerChanged(c);
	}
}

void FuckupCtrl::onDataReceive(Command c)
{
	ui->console->appendPlainText(tr("Chan: Command received - ") + CommandParser::toString(c));
}

void FuckupCtrl::on_uartOpenClose_clicked(void)
{
	if (uartChannel->isOpen()) {
		uartChannel->close();
	} else {
		setUartEnable(false);
		uartChannel->setDevice(ui->uartPort->currentText());
		uartChannel->setBaudrate(ui->uartBaudrate->currentIndex());
		uartChannel->open();
	}
}

void FuckupCtrl::onUartConnected(void)
{
	ui->console->appendPlainText(tr("Uart: Opened channel using port ") + uartChannel->getPort() +
								 " with Baudrate " + QString::number(uartChannel->getBaudrate()));
	ui->uartOpenClose->setText(tr("Close"));
	ui->uartOpenClose->setEnabled(true);
}

void FuckupCtrl::onUartDisconnected(void)
{
	setUartEnable(true);
	ui->uartOpenClose->setText(tr("Open"));
	ui->console->appendPlainText(tr("Uart: Closed channel"));
}

void FuckupCtrl::onUartError(QString mssg)
{
	setUartEnable(true);
	ui->uartOpenClose->setText(tr("Open"));
	ui->console->appendPlainText(tr("Uart: Exception on channel: ") + mssg);
}

void FuckupCtrl::onUartDeviceChanged(void)
{
	ui->uartPort->clear();
	foreach (QString port,uartChannel->getDevices()) {
		ui->uartPort->addItem(port);
	}
}


void FuckupCtrl::on_tcpOpenClose_clicked(void)
{
	if (tcpChannel->isOpen()) {
		tcpChannel->close();
	} else {
		setTcpEnable(false);
		tcpChannel->setHost(ui->tcpHost->text());
		tcpChannel->setPort(ui->tcpPort->text().toInt());
		tcpChannel->open();
	}
}

void FuckupCtrl::onTcpConnected(void)
{
	ui->console->appendPlainText(tr("TCP:  Channel opened to ") +
								 tcpChannel->getHost() + ":" + QString::number(tcpChannel->getPort()));
	ui->tcpOpenClose->setText(tr("Close"));
	ui->tcpOpenClose->setEnabled(true);

}

void FuckupCtrl::onTcpDisconnected(void)
{
	ui->tcpOpenClose->setText(tr("Open"));
	setTcpEnable(true);
	ui->console->appendPlainText(tr("TCP:  Channel closed"));
}

void FuckupCtrl::onTcpError(QString mssg)
{
	ui->tcpOpenClose->setText(tr("Open"));
	setTcpEnable(true);
	ui->console->appendPlainText(tr("TCP:  Exception on channel: ") + mssg);
}


void FuckupCtrl::on_actionQuit_triggered(void)
{
	qApp->exit();
}

void FuckupCtrl::on_actionDataSink_triggered(void)
{
	qDebug() << "Debug!";
    if (datasinkWidget == NULL) {
        datasinkWidget = new DataSink();
	}
    datasinkWidget->show();
}

void FuckupCtrl::on_actionAbout_triggered(void)
{
	QMessageBox mssg(this);
	mssg.setStandardButtons(QMessageBox::Close);
	mssg.setTextFormat(Qt::RichText);
	mssg.setText(tr("<span style=\"font-size:large; font-weight:bold\">Fuckup Control</span><br/>"
					"<span style=\"font-weight:bold\">Because nonesense matters!</span><br/><br/>"
					"Version: 0.23<br/><br/>"
					"Author:  Hauke Petersen (2013)<br/><br/>"
					"The program is provided AS fucked up as it IS with NO WARRANTY OF "
					"FUNCTION, BLINKING FANCY LIGHTS AND SAFETY, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY "
					"AND FITNESS FOR A PARTICULAR PURPOSE. IT MAY CONTAIN NUDITY AND HARD LANGUAGE, PARANTIAL SUPERVISION IS ADVISED."));
	mssg.setIconPixmap(QPixmap(":/icons/images/icon.png").scaledToWidth(100));
	mssg.exec();
}

void FuckupCtrl::on_actionInputConfig_triggered(void)
{
    if (inputConfigWidget == NULL) {
        inputConfigWidget = new InputConfig();
    }
    inputConfigWidget->show();
}

void FuckupCtrl::on_actionTestCommander_triggered(void)
{
	if (testCommanderWidget == 0) {
		testCommanderWidget = new TestCommander();
	}
	testCommanderWidget->show();
}

void FuckupCtrl::on_brake_clicked(void)
{
	Command c = CommandParser::parseMotorWrite(0, COMM_MOT_STAT_BRK, 0);
	emit speedChanged(c);
}

void FuckupCtrl::on_sw0_clicked(void)
{
	switches[0] = !switches[0];
	emit switchChanged(CommandParser::parseSwitchWrite(0, switches[0]));
	if (switches[0]) {
		ui->sw0->setText(tr("Off"));
	} else {
		ui->sw0->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw1_clicked(void)
{
	switches[1] = !switches[1];
	emit switchChanged(CommandParser::parseSwitchWrite(1, switches[1]));
	if (switches[1]) {
		ui->sw1->setText(tr("Off"));
	} else {
		ui->sw1->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw2_clicked(void)
{
	switches[2] = !switches[2];
	emit switchChanged(CommandParser::parseSwitchWrite(2, switches[2]));
	if (switches[2]) {
		ui->sw2->setText(tr("Off"));
	} else {
		ui->sw2->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw3_clicked(void)
{
	switches[3] = !switches[3];
	emit switchChanged(CommandParser::parseSwitchWrite(3, switches[3]));
	if (switches[3]) {
		ui->sw3->setText(tr("Off"));
	} else {
		ui->sw3->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw4_clicked(void)
{
	switches[4] = !switches[4];
	emit switchChanged(CommandParser::parseSwitchWrite(4, switches[4]));
	if (switches[4]) {
		ui->sw4->setText(tr("Off"));
	} else {
		ui->sw4->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw5_clicked(void)
{
	switches[5] = !switches[5];
	emit switchChanged(CommandParser::parseSwitchWrite(5, switches[5]));
	if (switches[5]) {
		ui->sw5->setText(tr("Off"));
	} else {
		ui->sw5->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw6_clicked(void)
{
	switches[6] = !switches[6];
	emit switchChanged(CommandParser::parseSwitchWrite(6, switches[6]));
	if (switches[6]) {
		ui->sw6->setText(tr("Off"));
	} else {
		ui->sw6->setText(tr("On"));
	}
}

void FuckupCtrl::on_sw7_clicked(void)
{
	switches[7] = !switches[7];
	emit switchChanged(CommandParser::parseSwitchWrite(7, switches[7]));
	if (switches[7]) {
		ui->sw7->setText(tr("Off"));
	} else {
		ui->sw7->setText(tr("On"));
	}
}


void FuckupCtrl::on_tcpEnable_stateChanged(void)
{
	if (ui->tcpEnable->isChecked()) {
		connectTcp();
	} else {
		disconnectTcp();
	}
}

void FuckupCtrl::on_uartEnable_stateChanged(void)
{
	if (ui->uartEnable->isChecked()) {
		connectUart();
	} else {
		disconnectUart();
	}
}

void FuckupCtrl::on_joyOpen_clicked(void)
{
	qDebug() << "Trying to open joystick";
	if (joydev.isOpen()) {
		joydev.close();
		ui->joyOpen->setText(tr("Open"));
	} else {
		joydev.open(ui->joyDev->currentIndex());
	}
}

void FuckupCtrl::onJoydevOpened(void) {
	ui->joyOpen->setText(tr("Close"));
}

void FuckupCtrl::onButton1Press(void) {
	qDebug() << "Button 1 year";
}

void FuckupCtrl::onJoydevUpdate(void)
{
	qDebug() << "Joystick update" << joydev.getJoydevs().size();
	if (!joydev.isOpen()) {
		ui->joyDev->clear();
		foreach (QString s, joydev.getJoydevs()) {
			ui->joyDev->addItem(s);
		}
	}
}

void FuckupCtrl::keyPressEvent(QKeyEvent *event)
{
	if (!event->isAutoRepeat()) {
		switch (event->key()) {
		case Qt::Key_Space:
			on_brake_clicked();
			break;
		case Qt::Key_Up:
			onSpeedChange(130);
			break;
		case Qt::Key_Down:
			onSpeedChange(-130);
			break;
		case Qt::Key_Left:
			onSteerChange(STEER_R);
			break;
		case Qt::Key_Right:
			onSteerChange(STEER_L);
			break;
		case Qt::Key_0:
			on_sw0_clicked();
			break;
		case Qt::Key_1:
			on_sw1_clicked();
			break;
		case Qt::Key_2:
			on_sw2_clicked();
			break;
		case Qt::Key_3:
			on_sw3_clicked();
			break;
		case Qt::Key_4:
			on_sw4_clicked();
			break;
		case Qt::Key_5:
			on_sw5_clicked();
			break;
		case Qt::Key_6:
			on_sw6_clicked();
			break;
		case Qt::Key_7:
			on_sw7_clicked();
			break;
		default:
			event->ignore();
		}
	}
}

void FuckupCtrl::keyReleaseEvent(QKeyEvent *event)
{
	if (!event->isAutoRepeat()) {
		switch (event->key()) {
		case Qt::Key_Up:
			onSpeedChange(SPEED_STOP);
			break;
		case Qt::Key_Down:
			onSpeedChange(SPEED_STOP);
			break;
		case Qt::Key_Left:
			onSteerChange(STEER_C);
			break;
		case Qt::Key_Right:
			onSteerChange(STEER_C);
			break;
		default:
			event->ignore();
		}
	}
}

void FuckupCtrl::closeEvent(QCloseEvent *)
{
	on_actionQuit_triggered();
}

void FuckupCtrl::setTcpEnable(bool enabled)
{
	ui->tcpOpenClose->setEnabled(enabled);
	ui->tcpHost->setEnabled(enabled);
	ui->tcpPort->setEnabled(enabled);
}

void FuckupCtrl::setUartEnable(bool enabled)
{
	ui->uartOpenClose->setEnabled(enabled);
	ui->uartBaudrate->setEnabled(enabled);
	ui->uartPort->setEnabled(enabled);
}

void FuckupCtrl::testPlot(void)
{
	qDebug() << "Test me";
	double x[101];
	double y[101];

	for ( int i = 0; i < 101; i++ ) {
		x[i] =  i / 10.0;
		y[i] = sin(x[i]);
	}

	qDebug() << "Creating plot objects now";
	plot = new QwtPlot(ui->centralWidget);

	// setup plot
	plot->setTitle(tr("Hello World"));
	plot->setCanvasBackground(QColor(Qt::white));
	plot->setFrameStyle(QFrame::Box|QFrame::Sunken);
	plot->setAxisTitle(plot->xBottom, tr("X Data"));
	plot->setAxisTitle(plot->yRight, tr("Acceleration"));

	QwtLegend *legend = new QwtLegend();
	legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
	plot->insertLegend(legend, QwtPlot::BottomLegend);

	qDebug() << "Plotting curve now";
	plot->setMinimumSize(QSize(500, 500));
	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setRawSamples(x, y, 101);

	plot->setAxisScale(plot->xBottom, 0.0, 10.0d, 0.0d);
	plot->setAxisAutoScale(plot->yLeft, true);
	plot->replot();

}


void FuckupCtrl::connectUart(void)
{
	connect(this, &FuckupCtrl::speedChanged, uartChannel, &UartSocket::send);
	connect(this, &FuckupCtrl::steerChanged, uartChannel, &UartSocket::send);
	connect(this, &FuckupCtrl::switchChanged, uartChannel, &UartSocket::send);
}

void FuckupCtrl::disconnectUart(void)
{
	disconnect(this, &FuckupCtrl::speedChanged, uartChannel, &UartSocket::send);
	disconnect(this, &FuckupCtrl::steerChanged, uartChannel, &UartSocket::send);
	disconnect(this, &FuckupCtrl::switchChanged, uartChannel, &UartSocket::send);
}

void FuckupCtrl::connectTcp(void)
{
	connect(this, &FuckupCtrl::speedChanged, tcpChannel, &TcpSocket::send);
	connect(this, &FuckupCtrl::steerChanged, tcpChannel, &TcpSocket::send);
	connect(this, &FuckupCtrl::switchChanged, tcpChannel, &TcpSocket::send);
}

void FuckupCtrl::disconnectTcp(void)
{
	disconnect(this, &FuckupCtrl::speedChanged, tcpChannel, &TcpSocket::send);
	disconnect(this, &FuckupCtrl::steerChanged, tcpChannel, &TcpSocket::send);
	disconnect(this, &FuckupCtrl::switchChanged, tcpChannel, &TcpSocket::send);
}
