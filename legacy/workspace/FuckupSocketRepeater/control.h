#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>

class Control : public QObject
{
	Q_OBJECT

private:
	qint16 speed;
	qint16 direction;

public:
	explicit Control(QObject *parent = 0);
	
signals:
	void speedChanged(void);
	void directionChanged(void);
	
public slots:
	void changeSpeed(qint16 speed);
	void changeDirection(qint16 direction);
	
};

#endif // CONTROL_H
