#include "wifi_module.h"
#include "app_wifi.h"
#include "rak_global.h"
#include "rak411_api.h"
#include "rak_config.h"
//Status������
OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
OS_Q WIFI_Msg;
uint32 	 	DIST_IP;
extern rak_CmdRsp	 uCmdRspFrame;
volatile 	rak_intStatus		rak_strIntStatus;
uint8 	 	Send_RecieveDataFlag=RAK_FALSE;
uint8_t     tx_buffer[1400]; 
static uint8_t TxBuffer[] = "****SPI - Two Boards communication based on Interrupt **** SPI Message ******** SPI Message ******** SPI Message ****";

OS_TMR	tmr1;	//����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����

void WIFI_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	int i;
	void *msg;
	OS_MSG_SIZE size;
	_WIFI_MSG *msg_wifi;
	rak_open_socket(LOCAL_PORT,DIST_PORT,RAK_MODULE_SOCKET_MODE,(uint32_t)DIST_IP);
		//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )200,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	OSTmrStart(&tmr1,&err);

	while(1)
	{
				//������ϢWIFI_Msg
		msg=OSQPend((OS_Q*			)&WIFI_Msg,   
					(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
		msg_wifi = (_WIFI_MSG *)msg;
		printf("msg_wifi->msg_type = %d\r\n",msg_wifi->msg_type);
		switch(msg_wifi->msg_type){
			case MSG_NOTIFY_GET_DATA:
						if(rak_checkPktIrq() == RAK_TRUE)
						{
							rak_read_packet(&uCmdRspFrame);
							rak_clearPktIrq();
							if(uCmdRspFrame.rspCode[0]==RSPCODE_RECV_DATA)
							{
								 Send_RecieveDataFlag=RAK_TRUE;			
							}
							printf("\nreceive data:");
							for(i = 0;i < uCmdRspFrame.recvFrame.data_len;i++)
							{
									printf("%x ",uCmdRspFrame.recvFrame.recvDataBuf[i]);
							}
						} 
			case MSG_NOTIFY_SEND_DATA:
						rak_send_data(0,0,uCmdRspFrame.recvFrame.socket_flag, 
						sizeof(TxBuffer),TxBuffer);					
				break;
			default:
				break;
		}

	}
}
//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr,void *p_arg)
{
	_WIFI_MSG *msg_wifi;
	msg_wifi->msg_type = MSG_NOTIFY_SEND_DATA;
	OS_ERR err;
	OSQPost((OS_Q*		)&WIFI_Msg,		
		(void*		)msg_wifi,
		(OS_MSG_SIZE)1,
		(OS_OPT		)OS_OPT_POST_FIFO,
		(OS_ERR*	)&err);

}

