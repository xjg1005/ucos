#ifndef _APP_WIFI_H
#define _APP_WIFI_H
#include "app_config.h"
#define WIFIMSG_Q_NUM	1	//������Ϣ���е�����
extern OS_Q WIFI_Msg;				//����һ����Ϣ����

extern OS_TCB WIFITaskTCB;
extern CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
extern void WIFI_task(void *p_arg);
enum msg_type{
	MSG_NOTIFY_GET_DATA = 0,
	MSG_NOTIFY_SEND_DATA,
};
typedef struct {
	 uint8_t msg_type;
	 uint8_t *data;
} _WIFI_MSG;
#endif 
