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
};
typedef struct {
	 uint8_t msg_type;
	 uint8_t *data;
} _VEHICLE_CTL_MSG;

#endif
