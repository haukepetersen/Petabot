#ifndef FUCKUPCTRL_H
#define FUCKUPCTRL_H

#include <QMainWindow>
#include <qwt_plot.h>
#include "datasink.h"
#include "inputconfig.h"
#include "testcommander.h"
#include "tcpsocket.h"
#include "uartsocket.h"
#include "joydev.h"

namespace Ui {
class FuckupCtrl;
}

class FuckupCtrl : public QMainWindow
{
	Q_OBJECT

private:
	Ui::FuckupCtrl *ui;
    DataSink *datasinkWidget;
    InputConfig *inputConfigWidget;
	TestCommander *testCommanderWidget;
	QwtPlot *plot;
	int speed;
	int steer;
	bool switches[8];
	Joydev joydev;

	// communication channels
	TcpSocket *tcpChannel;
	UartSocket *uartChannel;
	
public:
	explicit FuckupCtrl(QWidget *parent = 0);
	~FuckupCtrl();

signals:
	void speedChanged(Command c);
	void steerChanged(Command c);
	void switchChanged(Command c);

private slots:
	void onSpeedChange(int speed);
	void onSteerChange(int dir);

	void onDataReceive(Command c);


	void onUartConnected(void);
	void onUartDisconnected(void);
	void onUartError(QString mssg);
	void onUartDeviceChanged(void);

	void onTcpConnected(void);
	void onTcpDisconnected(void);
	void onTcpError(QString mssg);

	void on_actionAbout_triggered(void);
	void on_actionQuit_triggered(void);
	void on_actionDataSink_triggered(void);
	void on_actionInputConfig_triggered(void);
	void on_actionTestCommander_triggered(void);

	void on_uartOpenClose_clicked(void);
	void on_tcpOpenClose_clicked(void);

	void on_brake_clicked(void);
	void on_sw0_clicked(void);
	void on_sw1_clicked(void);
	void on_sw2_clicked(void);
	void on_sw3_clicked(void);
	void on_sw4_clicked(void);
	void on_sw5_clicked(void);
	void on_sw6_clicked(void);
	void on_sw7_clicked(void);

	void on_tcpEnable_stateChanged(void);
	void on_uartEnable_stateChanged(void);

	void on_joyOpen_clicked(void);
	void onJoydevOpened(void);
	void onButton1Press(void);
	void onJoydevUpdate(void);

protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *);

private:
	void setTcpEnable(bool enabled);
	void setUartEnable(bool enabled);
	void testPlot();
	void connectUart(void);
	void disconnectUart(void);
	void connectTcp(void);
	void disconnectTcp(void);

};

#endif // FUCKUPCTRL_H
