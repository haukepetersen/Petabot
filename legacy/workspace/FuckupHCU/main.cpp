#include <QCoreApplication>
#include <QDebug>
#include <fuckuphcu.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	FuckupHCU hcu(&a);
	if (hcu.start()) {
		return a.exec();
	}
}
