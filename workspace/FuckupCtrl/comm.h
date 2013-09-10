#ifndef COMM_H
#define COMM_H


// General command message fields
#define COMM_CMD_MASK			0x78
#define COMM_CMD_SHIFT			3
#define COMM_SIZE_MASK			0x07
#define COMM_SIZE_SHIFT			0
#define COMM_RW_MASK			0x80
#define COMM_RW_SHIFT			7
#define COMM_RW_READ			0
#define COMM_RW_WRITE			1

// Motor specific fields
#define COMM_MOT_CMD			0x01
#define COMM_MOT_RSIZE			1
#define COMM_MOT_WSIZE			2
#define COMM_MOT_CHAN_MASK		0xf0
#define COMM_MOT_CHAN_SHIFT		4
#define COMM_MOT_STAT_MASK		0x03
#define COMM_MOT_STAT_SHIFT		0
#define COMM_MOT_STAT_FW		0x02
#define COMM_MOT_STAT_BW		0x01
#define COMM_MOT_STAT_FREE		0x00
#define COMM_MOT_STAT_BRK		0x03

// Servo specific fields
#define COMM_SERV_CMD			0x02
#define COMM_SERV_RSIZE			1
#define COMM_SERV_WSIZE			2
#define COMM_SERV_CHAN_MASK		0xf0
#define COMM_SERV_CHAN_SHIFT	4
#define COMM_SERV_STAT_MASK		0x03
#define COMM_SERV_STAT_SHIFT	0
#define COMM_SERV_STAT_LEFT		0x01
#define COMM_SERV_STAT_RIGHT	0x02
#define COMM_SERV_STAT_CENTER	0x03

// Switching command fields
#define COMM_SWC_CMD			0x06
#define COMM_SWC_RSIZE			1
#define COMM_SWC_WSIZE			1
#define COMM_SWC_CHAN_MASK		0xfe
#define COMM_SWC_CHAN_SHIFT		1
#define COMM_SWC_STAT_MASK		0x01
#define COMM_SWC_STAT_SHIFT		0

// Status command fields
#define COMM_STAT_CMD			0x0f
#define COMM_STAT_RSIZE			0
#define COMM_STAT_WSIZE			1


typedef enum {
	CMD_R = 0x00,
	CMD_W = 0x01
} comm_rw_t;


typedef enum {
	CMD_UNDEFINED0 = 0,
	CMD_MOTOR = 1,
	CMD_SERVO,
	CMD_UNDEFINED3,
	CMD_UNDEFINED4,
	CMD_UNDEFINED5,
	CMD_SWITCH,
	CMD_CURRENT,
	CMD_ACCEL,
	CMD_GYRO,
	CMD_COMPAS,
	CMD_UNDEFINED11,
	CMD_UNDEFINED12,
	CMD_UNDEFINED13,
	CMD_HCUCONF,
	CMD_STATUS
} comm_type_t;


#endif // COMM_H
