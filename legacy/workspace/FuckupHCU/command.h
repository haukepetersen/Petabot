#ifndef COMMAND_H
#define COMMAND_H

#include <QByteArray>
#include <QString>

#include "comm.h"

class Command
{
private:
    QByteArray data;

public:
	Command(QByteArray data);

	char* rawData(void);
	int length(void);

	comm_type_t getType(void);
	int getSize(void);
	char getData(int index);
	bool isRead(void);
	bool isWrite(void);

};

#endif // COMMAND_H
