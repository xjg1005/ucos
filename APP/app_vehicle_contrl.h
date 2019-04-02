#ifndef __VEHICLE_CONTRL_H
#define __VEHICLE_CONTRL_H
/* Includes ------------------------------------------------------------------*/
#include "app_config.h"

#define VehicleContrl_MSG_Q_NUM	1	//按键消息队列的数量
extern OS_Q VehicleContrl_Msg;				//定义一个消息队列

extern OS_TCB VehicleContrl_TaskTCB;
extern CPU_STK VehicleContrl_TASK_STK[VehicleContrl_STK_SIZE];
extern void VehicleContrl_task(void *p_arg);

enum vc_msg_type{
	MSG_NOTIFY_TURN_LEFT = 0xA0,
	MSG_NOTIFY_TURN_RIGHT,
	MSG_NOTIFY_ADVANCE,
	MSG_NOTIFY_BACKWARD,
	MSG_NOTIFY_ACCLERATE,
	MSG_NOTIFY_DECLERATE,
	MSG_NOTIFY_BREAK,
	MSG_NOTIFY_GET_DISTANCE,
};

enum vc_status_type{
	STATUS_TURN_LEFT = 0xB0,
	STATUS_TURN_RIGHT,
	STATUS_ADVANCE,
	STATUS_BACKWARD,
	STATUS_ACCLERATE,
	STATUS_DECLERATE,
	STATUS_BREAK,
	STATUS_GET_DISTANCE,
};
typedef struct {
	 uint8_t runnng_status;
	 uint8_t distance;
} _VEHICLE_SATUS;
typedef struct {
	 uint8_t msg_type;
	 void *data;
} _VEHICLE_CTL_MSG;

extern _VEHICLE_CTL_MSG vehicle_ctl_msg;
extern _VEHICLE_SATUS vehicle_status;
#endif
