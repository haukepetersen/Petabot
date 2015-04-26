#include "command.h"

Command::Command(QByteArray data)
{
    this->data = data;
}

int Command::length(void)
{
    return data.length();
}

char* Command::rawData(void)
{
    return data.data();
}

comm_type_t Command::getType(void)
{
	return (comm_type_t)((data[0] & COMM_CMD_MASK) >> COMM_CMD_SHIFT);
}

int Command::getSize(void)
{
	return (int)((data[0] & COMM_SIZE_MASK) >> COMM_SIZE_SHIFT);

}

char Command::getData(int index)
{
	if (index + 1 < data.size()) {
		return data[index + 1];
	}
	return 0;
}

bool Command::isRead(void)
{
	return (data[0] & COMM_RW_MASK) == 0;
}

bool Command::isWrite(void)
{
	return (data[0] & COMM_RW_MASK) > 0;
}
