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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*echo*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB0

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //PB0复用位定时器3

}
TIM_ICInitTypeDef  TIM3_ICInitStructure;

//定时器5通道1输入捕获配置
//arr：自动重装值(TIM3,TIM3是16位的!!)
//psc：时钟预分频数
void TIM3_CH3_Cap_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3时钟使能    
	  
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	

	//初始化TIM3输入捕获参数
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC3S=01 	选择输入端 IC1映射到TI1上
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
		
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
  TIM_Cmd(TIM3,ENABLE ); 	//使能定时器3

 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	
}
//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM3CH3_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM3CH3_CAPTURE_VAL;	//输入捕获值(TIM3/TIM3是16位)
//定时器5中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    

 	if((TIM3CH3_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//溢出
		{	     
			if(TIM3CH3_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM3CH3_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM3CH3_CAPTURE_STA|=0X80;		//标记成功捕获了一次
					TIM3CH3_CAPTURE_VAL=0XFFFF;
				}else TIM3CH3_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)//捕获1发生捕获事件
		{	
			if(TIM3CH3_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM3CH3_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			  TIM3CH3_CAPTURE_VAL=TIM_GetCapture3(TIM3);//获取当前的捕获值.
	 			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM3CH3_CAPTURE_STA=0;			//清空
				TIM3CH3_CAPTURE_VAL=0;
				TIM3CH3_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				TIM_Cmd(TIM3,DISABLE ); 	//关闭定时器3
	 			TIM_SetCounter(TIM3,0);
	 			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				TIM_Cmd(TIM3,ENABLE ); 	//使能定时器3
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位
}

void SR04_EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//PA2 连接到中断线2
	
  /* 配置EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE2
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE2
  EXTI_Init(&EXTI_InitStructure);//配置
	
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	   
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
	 EXTI_ClearITPendingBit(EXTI_Line0);//清除LINE2上的中断标志位 
	 busy_Flag = 0;
}

int get_Diatance(void)
{
	OS_ERR err;
	long long temp=0;  
	if(busy_Flag == 1)
	{
		if(TIM3CH3_CAPTURE_STA&0X80)        //成功捕获到了一次高电平
		{
			temp=TIM3CH3_CAPTURE_STA&0X3F; 
			temp*=0XFFFF;		 		         //溢出时间总和
			temp+=TIM3CH3_CAPTURE_VAL;		   //得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp); //打印总的高点平时间
			distance = temp*0.017;
			vehicle_ctl_msg.msg_type = MSG_NOTIFY_GET_DISTANCE;
			vehicle_ctl_msg.data = &distance;
			OSQPost((OS_Q*		)&VehicleContrl_Msg,		
							(void*		)&vehicle_ctl_msg,
							(OS_MSG_SIZE)1,
							(OS_OPT		)OS_OPT_POST_FIFO,
							(OS_ERR*	)&err);

			TIM3CH3_CAPTURE_STA=0;			     //开启下一次捕获
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
 	TIM3_CH3_Cap_Init(0XFFFF,84-1); //以1Mhz的频率计数 
 		delay_ms(10);	
}
