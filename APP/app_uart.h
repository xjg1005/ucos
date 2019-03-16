#ifndef _APP_UART_H
#define _APP_UART_H
#include "app_config.h"
#define UARTMSG_Q_NUM	1	//按键消息队列的数量
#define PACKAG_LEN    1024
extern OS_Q UART_Msg;				//定义一个消息队列

extern OS_TCB UARTTaskTCB;
extern CPU_STK UART_TASK_STK[UART_STK_SIZE];
extern void UART_task(void *p_arg);
extern void UART_send_data(u32 len,uint8_t *data);
enum msg_type{
	MSG_NOTIFY_GET_DATA = 0,
	MSG_NOTIFY_SEND_DATA,
};

enum msg_contrl{
	MSG_VEHICLE_CONTRL = 0xc0,
};

typedef struct {
	 uint8_t msg_type;
	 uint8_t *data;
} _UART_MSG;
#endif 
