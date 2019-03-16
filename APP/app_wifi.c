#include "app_wifi.h"
#include "app_vehicle_contrl.h"
//Status任务函数
OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
OS_Q WIFI_Msg;

volatile uint8_t msg_wifi;

OS_TMR	tmr1;	//定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg); //定时器1回调函数

void wifi_send_data(u32 len,uint8_t *data)
{

}
void WIFI_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	int i;
	void *msg;
	OS_MSG_SIZE size;
	uint8_t *msg_wifi;
		//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",		//定时器名字
                (OS_TICK	 )500,			//0ms
                (OS_TICK	 )200,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
	OSTmrStart(&tmr1,&err);

	while(1)
	{
				//请求消息WIFI_Msg
		msg=OSQPend((OS_Q*			)&WIFI_Msg,   
					(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
		printf("WIFI_Msg\r\n");
		msg_wifi = (uint8_t *)msg;
		switch(*msg_wifi){
			case MSG_NOTIFY_GET_DATA:
						
						break;
			case MSG_NOTIFY_SEND_DATA:
											
				break;
			default:
				break;
		}

	}
}
//定时器1的回调函数
void tmr1_callback(void *p_tmr,void *p_arg)
{
	msg_wifi = MSG_NOTIFY_SEND_DATA;
	OS_ERR err;
//	OSQPost((OS_Q*		)&WIFI_Msg,		
//		(void*		)&msg_wifi,
//		(OS_MSG_SIZE)1,
//		(OS_OPT		)OS_OPT_POST_FIFO,
//		(OS_ERR*	)&err);

}

