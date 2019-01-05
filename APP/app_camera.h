#ifndef __APP_CAMERA_H
#define __APP_CAMERA_H
/* Includes ------------------------------------------------------------------*/
#include "app_config.h"

#define CAMERA_MSG_Q_NUM	1	//按键消息队列的数量
extern OS_Q Camera_Msg;				//定义一个消息队列

extern OS_TCB Camera_TaskTCB;
extern CPU_STK Camera_TASK_STK[Camera_STK_SIZE];
extern void Camera_task(void *p_arg);


enum camera_msg_type{
	MSG_NOTIFY_HANDLE_CAMERA_DATA = 0xA0,
};
typedef struct {
	 uint8_t msg_type;
	 uint8_t *data;
} _CAMERA_MSG;
#endif