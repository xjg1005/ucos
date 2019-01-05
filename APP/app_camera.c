#include "app_camera.h"
#include "usart2.h"  
#include "ov2640.h" 
#include "dcmi.h" 
#include "app_wifi.h"
OS_TCB Camera_TaskTCB;
CPU_STK Camera_TASK_STK[VehicleContrl_STK_SIZE];
OS_Q Camera_Msg;


u8 ov2640_mode=1;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ

#define jpeg_buf_size 10*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
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
char *EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
char *JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEGͼƬ 9�ֳߴ� 
_CAMERA_MSG camera_msg_common;


//us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
void camera_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//ʹ��UCOSIII
	OS_ERR err; 
	OSSchedLock(&err);						//UCOSIII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#else										//����UCOSII
	OSSchedLock();							//UCOSII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#endif
}

//us����ʱʱ,�ָ��������
void camera_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   			//ʹ��UCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err);					//UCOSIII�ķ�ʽ,�ָ�����
#else										//����UCOSII
	OSSchedUnlock();						//UCOSII�ķ�ʽ,�ָ�����
#endif
}
//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
		OS_ERR err;
	camera_osschedlock();
	if(ov2640_mode)//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//�õ��˴����ݴ���ĳ���
				
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
			camera_msg_common.msg_type = MSG_NOTIFY_HANDLE_CAMERA_DATA;
			OSQPost((OS_Q*		)&Camera_Msg,		
			(void*		)&camera_msg_common,
			(OS_MSG_SIZE)1,
			(OS_OPT		)OS_OPT_POST_FIFO,
			(OS_ERR*	)&err);

		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
	}
	camera_osschedunlock();
} 
//JPEG����
//JPEG����,ͨ������2���͸�����.
void jpeg_init(void)
{
	u32 i; 
	u8 *p;
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 size=3;		//Ĭ����QVGA 320*240�ߴ�
	u8 msgbuf[15];	//��Ϣ������ 
		printf("jpeg_init\r\n");

	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	
 	OV2640_JPEG_Mode();		//JPEGģʽ
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA����   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	DCMI_Start(); 		//��������
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
						if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
						{  
							p=(u8*)jpeg_buf;
//							for(i=0;i<jpeg_data_len*4;i++)		//dma����1�ε���4�ֽ�,���Գ���4.
//							{
//								while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);	//ѭ������,ֱ���������  		
//								USART_SendData(USART1,p[i]); 
//							} 
							wifi_send_data(jpeg_data_len*4,p);
							camera_osschedunlock();
							jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
						}	
						break;
					default:
						break;
				}

	}
}

