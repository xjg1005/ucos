#include "sr04.h"
#include "app_config.h"
#include "app_vehicle_contrl.h"
static uint8_t TIM2_Flag = 0;
static uint8_t busy_Flag = 0;
uint8_t distance = 0;
static   u16 TIM = 0;
static _VEHICLE_CTL_MSG *vehicle_ctrl_msg;
void GPIO_Configuration()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//  /*echo*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /*trig*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*echo*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB0

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //PB0����λ��ʱ��3

}
TIM_ICInitTypeDef  TIM3_ICInitStructure;

//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM3,TIM3��16λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM3_CH3_Cap_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3ʱ��ʹ��    
	  
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	

	//��ʼ��TIM3���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC3S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
		
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
  TIM_Cmd(TIM3,ENABLE ); 	//ʹ�ܶ�ʱ��3

 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	
}
//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM3CH3_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM3CH3_CAPTURE_VAL;	//���벶��ֵ(TIM3/TIM3��16λ)
//��ʱ��5�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    

 	if((TIM3CH3_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//���
		{	     
			if(TIM3CH3_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM3CH3_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM3CH3_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					TIM3CH3_CAPTURE_VAL=0XFFFF;
				}else TIM3CH3_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)//����1���������¼�
		{	
			if(TIM3CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM3CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			  TIM3CH3_CAPTURE_VAL=TIM_GetCapture3(TIM3);//��ȡ��ǰ�Ĳ���ֵ.
	 			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM3CH3_CAPTURE_STA=0;			//���
				TIM3CH3_CAPTURE_VAL=0;
				TIM3CH3_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM_Cmd(TIM3,DISABLE ); 	//�رն�ʱ��3
	 			TIM_SetCounter(TIM3,0);
	 			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				TIM_Cmd(TIM3,ENABLE ); 	//ʹ�ܶ�ʱ��3
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC3|TIM_IT_Update); //����жϱ�־λ
}

void SR04_EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//PA2 ���ӵ��ж���2
	
  /* ����EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE2
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE2
  EXTI_Init(&EXTI_InitStructure);//����
	
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	   
}
void EXTI0_IRQHandler(void)
{
	 if((!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) /*&& TIM2_Flag==0*/)
	 {
		 printf("faling \r\n");
		   TIM_Cmd(TIM3, ENABLE);            
			TIM3->CNT = 0;   
	 }
	 else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)/*&& TIM2_Flag==0*/)
	 {
		 		 printf("rising \r\n");
			TIM_Cmd(TIM3, DISABLE);    
//			if(TIM2_Flag==1)
//				TIM2_Flag = 0;
  
			TIM = TIM3->CNT;
			distance = TIM*0.85;
		 printf("TIM = %d \r\n",TIM);
	 }
	 EXTI_ClearITPendingBit(EXTI_Line0);//���LINE2�ϵ��жϱ�־λ 
	 busy_Flag = 0;
}

int get_Diatance(void)
{
	OS_ERR err;
	long long temp=0;  
	if(busy_Flag == 1)
	{
		if(TIM3CH3_CAPTURE_STA&0X80)        //�ɹ�������һ�θߵ�ƽ
		{
			temp=TIM3CH3_CAPTURE_STA&0X3F; 
			temp*=0XFFFF;		 		         //���ʱ���ܺ�
			temp+=TIM3CH3_CAPTURE_VAL;		   //�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%lld us\r\n",temp); //��ӡ�ܵĸߵ�ƽʱ��
			distance = temp*0.017;
			vehicle_ctl_msg.msg_type = MSG_NOTIFY_GET_DISTANCE;
			vehicle_ctl_msg.data = &distance;
			OSQPost((OS_Q*		)&VehicleContrl_Msg,		
							(void*		)&vehicle_ctl_msg,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);

			TIM3CH3_CAPTURE_STA=0;			     //������һ�β���
			busy_Flag = 0;
		}
	 return -1;  
	}
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  GPIO_SetBits(GPIOB, GPIO_Pin_1);  
  delay_us(30);
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	busy_Flag = 1;
  return 0;
}



void sr04_init(void)
{
	GPIO_Configuration();
 	TIM3_CH3_Cap_Init(0XFFFF,84-1); //��1Mhz��Ƶ�ʼ��� 
 		delay_ms(10);	
}
