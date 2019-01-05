#include "wifi_module.h"
#include "app_wifi.h"
#include "rak_global.h"
#include "rak411_api.h"
#include "rak_config.h"
#include "app_vehicle_contrl.h"
//Status任务函数
OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
OS_Q WIFI_Msg;
uint32 	 	DIST_IP;
extern rak_CmdRsp	 uCmdRspFrame;
volatile 	rak_intStatus		rak_strIntStatus;
uint8 	 	Send_RecieveDataFlag=RAK_FALSE;
uint8_t     tx_buffer[1400]; 
static uint8_t TxBuffer[] = "****SPI - Two Boards communication based on Interrupt **** SPI Message ******** SPI Message ******** SPI Message ****";
volatile uint8_t msg_wifi;

OS_TMR	tmr1;	//定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg); //定时器1回调函数

void wifi_send_data(u32 len,uint8_t *data)
{
	u16 packge_num = len/PACKAG_LEN;
	u16 last_len = len%PACKAG_LEN;
	u16 i=0;
	printf("len=%d,packge_num=%d,last_len=%d\r\n",len,packge_num,last_len);
	if(packge_num>0){
		for(i=0;i<packge_num;i++)
		{
			rak_send_data(0,0,uCmdRspFrame.recvFrame.socket_flag, PACKAG_LEN,data+i*PACKAG_LEN);
			printf("rak_send_data cnt =%d\r\n",i);
		}
		
	}
	if(last_len>0)
		rak_send_data(0,0,uCmdRspFrame.recvFrame.socket_flag, 
					last_len,data+i*PACKAG_LEN);
}
void WIFI_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	int i;
	void *msg;
	OS_MSG_SIZE size;
	uint8_t *msg_wifi;
	rak_open_socket(LOCAL_PORT,DIST_PORT,RAK_MODULE_SOCKET_MODE,(uint32_t)DIST_IP);
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
						if(rak_checkPktIrq() == RAK_TRUE)
						{
							rak_read_packet(&uCmdRspFrame);
							rak_clearPktIrq();
							if(uCmdRspFrame.rspCode[0]==RSPCODE_RECV_DATA)
							{
								 Send_RecieveDataFlag=RAK_TRUE;			
							}
							for(i = 0;i < uCmdRspFrame.recvFrame.data_len;i++)
							{
									printf("%x ",uCmdRspFrame.recvFrame.recvDataBuf[i]);
							}
							if(uCmdRspFrame.recvFrame.recvDataBuf[0] == MSG_VEHICLE_CONTRL){
										vehicle_ctl_msg.msg_type = uCmdRspFrame.recvFrame.recvDataBuf[1];
										vehicle_ctl_msg.data = uCmdRspFrame.recvFrame.recvDataBuf;
										OSQPost((OS_Q*		)&VehicleContrl_Msg,		
														(void*		)&vehicle_ctl_msg,
														(OS_MSG_SIZE)1,
														(OS_OPT		)OS_OPT_POST_FIFO,
														(OS_ERR*	)&err);

							}
						} 
						break;
			case MSG_NOTIFY_SEND_DATA:
						wifi_send_data(sizeof(TxBuffer),TxBuffer);					
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

