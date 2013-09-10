#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QObject>
#include <QByteArray>
#include "command.h"

class CommandParser : public QObject
{
    Q_OBJECT

private:
    QByteArray buffer;
    int size;
    int count;


public:
	explicit CommandParser(QObject *parent = 0);

	static Command parseMotorRead(int channel);
	static Command parseMotorWrite(int channel, int mode, int value);
	static Command parseMotorWrite(int channel, int value);
	static Command parseServoRead(int channel);
	static Command parseServoWrite(int channel, int mode, int value);
	static Command parseServoWrite(int channel, int value);
	static Command parseSwitchRead(int channel);
	static Command parseSwitchWrite(int channel, bool enabled);
	static Command parseStatusRead();
	static Command parseStatusWrite(int id);

	static QString toString(Command c);
	static QString typeToString(comm_type_t type);
	static QString c2hex(char c, int stuff = 2);
    
signals:
    void commandReady(Command c);
    
public slots:
    void onNewData(QByteArray data);

private:
    bool waitingForHead(void);
	bool commandComplete(void);

	static QByteArray parseCommandHeader(comm_type_t type, int size, comm_rw_t rw);
    
};

#endif // COMMANDPARSER_H
