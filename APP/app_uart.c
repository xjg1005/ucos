#include "app_uart.h"
#include "app_vehicle_contrl.h"
//Status������
OS_TCB UARTTaskTCB;
CPU_STK UART_TASK_STK[UART_STK_SIZE];
OS_Q UART_Msg;

volatile uint8_t msg_UART;

OS_TMR	tmr1;	//����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����

void UART_send_data(u32 len,uint8_t *data)
{

}
void UART_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	void *msg;
	OS_MSG_SIZE size;
	uint8_t *msg_UART;
		//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )500,			//0ms
                (OS_TICK	 )200,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	OSTmrStart(&tmr1,&err);

	while(1)
	{
				//������ϢUART_Msg
		msg=OSQPend((OS_Q*			)&UART_Msg,   
					(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
		printf("UART_Msg\r\n");
		msg_UART = (uint8_t *)msg;
		switch(*msg_UART){
			case MSG_NOTIFY_GET_DATA:
						
						break;
			case MSG_NOTIFY_SEND_DATA:
											
				break;
			default:
				break;
		}

	}
}
//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	msg_UART = MSG_NOTIFY_SEND_DATA;
	//OS_ERR err;
//	OSQPost((OS_Q*		)&UART_Msg,		
//		(void*		)&msg_UART,
//		(OS_MSG_SIZE)1,
//		(OS_OPT		)OS_OPT_POST_FIFO,
//		(OS_ERR*	)&err);

}

