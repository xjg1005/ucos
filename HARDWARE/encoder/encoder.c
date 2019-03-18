#include "encoder.h"
#include "app_config.h"

void TIM5_ENC_Init(void)  //Moter_D
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʹ��TIM5ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

	/* Configure PA0,A1 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//����ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���Ÿ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//��©���
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);//��PA0��������ΪTIM5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);//��PA1��������ΪTIM5
	
	//�������ӿ�����
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM5->CNT=0;
	TIM_Cmd(TIM5, ENABLE); 
}
void TIM4_ENC_Init(void)  //Moter_C
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ��TIM4ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOBʱ��

	/* Configure PC.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//����ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���Ÿ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//��©���
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);//��PB6��������ΪTIM4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);//��PB7��������ΪTIM4
	
	//�������ӿ�����
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







