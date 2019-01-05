#include "app_camera.h"
#include "usart2.h"  
#include "ov2640.h" 
#include "dcmi.h" 
#include "app_wifi.h"
OS_TCB Camera_TaskTCB;
CPU_STK Camera_TASK_STK[VehicleContrl_STK_SIZE];
OS_Q Camera_Msg;


u8 ov2640_mode=1;						//工作模式:0,RGB565模式;1,JPEG模式

#define jpeg_buf_size 10*1024  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
//JPEG尺寸支持列表
u16 jpeg_img_size_tbl[][2]=
{
	176,144,	//QCIF
	160,120,	//QQVGA
	352,288,	//CIF
	320,240,	//QVGA
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,1024,	//SXGA
	1600,1200,	//UXGA
}; 
char *EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
char *JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEG图片 9种尺寸 
_CAMERA_MSG camera_msg_common;


//us级延时时,关闭任务调度(防止打断us级延迟)
void camera_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedLock(&err);						//UCOSIII的方式,禁止调度，防止打断us延时
#else										//否则UCOSII
	OSSchedLock();							//UCOSII的方式,禁止调度，防止打断us延时
#endif
}

//us级延时时,恢复任务调度
void camera_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
#else										//否则UCOSII
	OSSchedUnlock();						//UCOSII的方式,恢复调度
#endif
}
//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
		OS_ERR err;
	camera_osschedlock();
	if(ov2640_mode)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//停止当前传输 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//得到此次数据传输的长度
				
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
			camera_msg_common.msg_type = MSG_NOTIFY_HANDLE_CAMERA_DATA;
			OSQPost((OS_Q*		)&Camera_Msg,		
			(void*		)&camera_msg_common,
			(OS_MSG_SIZE)1,
			(OS_OPT		)OS_OPT_POST_FIFO,
			(OS_ERR*	)&err);

		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1, ENABLE);			//重新传输
			jpeg_data_ok=0;						//标记数据未采集
		}
	}
	camera_osschedunlock();
} 
//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_init(void)
{
	u32 i; 
	u8 *p;
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 size=3;		//默认是QVGA 320*240尺寸
	u8 msgbuf[15];	//消息缓存区 
		printf("jpeg_init\r\n");

	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	
 	OV2640_JPEG_Mode();		//JPEG模式
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA配置   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
} 
 

void Camera_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	OS_MSG_SIZE size;
	void *msg;
	_CAMERA_MSG *camera_msg;
		u32 i; 
	u8 *p;
	printf("Camera_task\r\n");
	jpeg_init();
	while(1)
	{
				msg=OSQPend((OS_Q*			)&Camera_Msg,   
											(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
			camera_msg = (_CAMERA_MSG *)msg;
				printf("camera_msg->msg_type=%d\r\n",camera_msg->msg_type);
				switch(camera_msg->msg_type){
					case MSG_NOTIFY_HANDLE_CAMERA_DATA:
						camera_osschedlock();
						if(jpeg_data_ok==1)	//已经采集完一帧图像了
						{  
							p=(u8*)jpeg_buf;
//							for(i=0;i<jpeg_data_len*4;i++)		//dma传输1次等于4字节,所以乘以4.
//							{
//								while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);	//循环发送,直到发送完毕  		
//								USART_SendData(USART1,p[i]); 
//							} 
							wifi_send_data(jpeg_data_len*4,p);
							camera_osschedunlock();
							jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
						}	
						break;
					default:
						break;
				}

	}
}

