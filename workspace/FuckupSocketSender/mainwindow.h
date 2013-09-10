#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QAbstractSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindow *ui;
	QTcpSocket *socket;
	bool connected;
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();


private slots:
	void receivedData(void);
	void print(QString txt);
	void sendData(void);
	void connectToServer(void);
	void onConnect(void);
	void onDisconnect();
	void onError(QAbstractSocket::SocketError error);
};

#endif // MAINWINDOW_H
