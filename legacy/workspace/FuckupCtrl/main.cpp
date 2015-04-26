#include "fuckupctrl.h"
#include <QApplication>

//#include <QDebug>
//#include <QByteArray>
//#include <QString>
//#include "commandparser.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FuckupCtrl w;
	w.show();
	
	return a.exec();
}
