#include "encoder.h"
#include "app_config.h"

void TIM5_ENC_Init(void)  //Moter_D
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //使能TIM5时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

	/* Configure PA0,A1 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//最高速度为100MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//引脚复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//开漏输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);//将PA0引脚配置为TIM5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);//将PA1引脚配置为TIM5
	
	//编码器接口配置
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM5->CNT=0;
	TIM_Cmd(TIM5, ENABLE); 
}
void TIM4_ENC_Init(void)  //Moter_C
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIM4时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOB时钟

	/* Configure PC.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//最高速度为100MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//引脚复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//开漏输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);//将PB6引脚配置为TIM4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);//将PB7引脚配置为TIM4
	
	//编码器接口配置
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM4->CNT=0;
	TIM_Cmd(TIM4, ENABLE); 
}
void Caculate_Encoder()
{
	extern int Encoder_Data[];
	if(TIM4->CR1&0x10)
	{
		Encoder_Data[0] = 0xffff - TIM4->CNT;
		TIM4->CNT = 0xffff;
	}
	else 
	{
		Encoder_Data[0] = TIM4->CNT;
		TIM4->CNT = 0;
		
	}
	if(TIM5->CR1&0x10)
	{
		Encoder_Data[1] = 0xffffffff - TIM5->CNT;
		TIM5->CNT = 0xffffffff;
	}
	else 
	{
		Encoder_Data[1] = TIM5->CNT;
		TIM5->CNT = 0;
		
	}
	printf("Encoder_Data[0]=%d,Encoder_Data[1]=%d\n",Encoder_Data[0],Encoder_Data[1]);
}
void encoder_init()
{
	TIM4_ENC_Init();
	TIM5_ENC_Init();
}







