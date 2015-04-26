#include "commandparser.h"
#include "comm.h"

CommandParser::CommandParser(QObject *parent) :
    QObject(parent)
{
	count = 0;
	size = 0;
}

Command CommandParser::parseMotorRead(int channel)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_MOTOR, 1, CMD_R);
	raw[1] = (channel << COMM_MOT_CHAN_SHIFT) & COMM_MOT_CHAN_MASK;
	return Command(raw);
}

Command CommandParser::parseMotorWrite(int channel, int mode, int value)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_MOTOR, 2, CMD_W);
	raw[1] = ((channel << COMM_MOT_CHAN_SHIFT) & COMM_MOT_CHAN_MASK)
			| ((mode << COMM_MOT_STAT_SHIFT) & COMM_MOT_STAT_MASK);
	raw[2] = value & 0xff;
	return Command(raw);
}

Command CommandParser::parseMotorWrite(int channel, int value)
{
	int mode;
	if (value < 0) {
		mode = COMM_MOT_STAT_BW;
		value = -value;
	} else {
		mode = COMM_MOT_STAT_FW;
	}
	return parseMotorWrite(channel, mode, value);
}

Command CommandParser::parseServoRead(int channel)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_SERVO, 1, CMD_R);
	raw[1] = ((channel << COMM_SERV_CHAN_SHIFT) & COMM_SERV_CHAN_MASK);
	return Command(raw);
}

Command CommandParser::parseServoWrite(int channel, int mode, int value)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_SERVO, 2, CMD_W);
	raw[1] = ((channel << COMM_SERV_CHAN_SHIFT) & COMM_SERV_CHAN_MASK)
			| ((mode << COMM_SERV_STAT_SHIFT) & COMM_SERV_STAT_MASK);
	raw[2] = value & 0xff;
	return Command(raw);
}

Command CommandParser::parseServoWrite(int channel, int value)
{
	int mode;
	if (value != 0) {
		mode = (value < 0) ? COMM_SERV_STAT_LEFT : COMM_SERV_STAT_RIGHT;
		value = (value < 0) ? -value : value;
	} else {
		mode = COMM_SERV_STAT_CENTER;
	}
	return parseServoWrite(channel, mode, value);
}

Command CommandParser::parseSwitchRead(int channel)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_SWITCH, 1, CMD_R);
	raw[1] = ((channel << COMM_SWC_CHAN_SHIFT) & COMM_SWC_CHAN_MASK);
	return Command(raw);
}

Command CommandParser::parseSwitchWrite(int channel, bool enabled)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_SWITCH, 1, CMD_W);
	raw[1] = ((channel << COMM_SWC_CHAN_SHIFT) & COMM_SWC_CHAN_MASK);
	raw[1] = (enabled) ? raw[1] | 0x01 : raw[1];
	return Command(raw);
}

Command CommandParser::parseStatusRead()
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_STATUS, 0, CMD_R);
	return Command(raw);
}

Command CommandParser::parseStatusWrite(int id)
{
	QByteArray raw = CommandParser::parseCommandHeader(CMD_STATUS, 1, CMD_W);
	raw[1] = id & 0xff;
	return Command(raw);
}


QString CommandParser::typeToString(comm_type_t type)
{
	switch (type) {
	case CMD_MOTOR:
		return QString("CMD_MOTOR");
	case CMD_SERVO:
		return QString("CMD_SERVO");
	case CMD_SWITCH:
		return QString("CMD_SWITCH");
	case CMD_CURRENT:
		return QString("CMD_CURRENT");
	case CMD_ACCEL:
		return QString("CMD_ACCEL");
	case CMD_GYRO:
		return QString("CMD_GYRO");
	case CMD_COMPAS:
		return QString("CMD_COMPAS");
	case CMD_HCUCONF:
		return QString("CMD_HCUCONF");
	case CMD_STATUS:
		return QString("CMD_STATUS");
	default:
		return QString("UNDEFINED");
	}
}

QString CommandParser::toString(Command c)
{
	QString rw = (c.isRead()) ? "R" : "W";
	QString res = QString(tr("Commmand: %1 %2 %3byte"))
			.arg(rw).arg(typeToString(c.getType()), 11).arg(c.getSize(), 2, 10);
	for (int i = 0; i < c.getSize(); i++) {
		res += QString(" | 0x%1").arg(CommandParser::c2hex(c.getData(i)));
	}
	return res;
}

QString CommandParser::c2hex(char c, int stuff)
{
	QString s = QString::number(c & 0xff, 16);
	while (s.size() < stuff) {
		s.prepend('0');
	}
	return s;
}

void CommandParser::onNewData(QByteArray data)
{
	for (int i = 0; i < data.length(); i ++) {
        char next = data.at(i);
		if (waitingForHead()) {
            size = ((int)((next & COMM_SIZE_MASK) >> COMM_SIZE_SHIFT)) + 1;
			buffer.resize(size);
		}
        buffer[count++] = next;
		if (commandComplete()) {
			count = 0;
            emit commandReady(Command(buffer));
        }
    }
}

bool CommandParser::waitingForHead(void)
{
    return count == 0;
}

bool CommandParser::commandComplete(void)
{
    return count == size;
}

QByteArray CommandParser::parseCommandHeader(comm_type_t type, int size, comm_rw_t rw)
{
	QByteArray res;
	char header = (char)((type << COMM_CMD_SHIFT) & COMM_CMD_MASK);
	header |= (char)((rw << COMM_RW_SHIFT) & COMM_RW_MASK);
	header |= (char)((size << COMM_SIZE_SHIFT) & COMM_SIZE_MASK);
	res.resize(size + 1);
	res[0] = header;
	return res;
}
