#include "motor.h"
#include "app_config.h"
#include "app_vehicle_contrl.h"

#define PWM_MAX   100
#define PWM_MIN  	45
extern uint8_t last_status;
u16 Encoder_Data[2]={0,0};
float Kp= 0.3,Ki=0.1,Kd = 0.5;
static void INT_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOFʱ��

  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = INT1_Pin | INT2_Pin | INT3_Pin| INT4_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��

}
static void TIM9_1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTDʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM9); //GPIOA9����Ϊ��ʱ��11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;           //GPIOD12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PD12
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//��ʼ����ʱ��2
	
	//��ʼ��TIM9 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2OC1

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM9��CCR1�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM9, ENABLE);  //ʹ��TIM9
//	TIM_CtrlPWMOutputs(TIM9,ENABLE);										  
										  
}  

static void TIM9_2_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTDʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM9); //GPIOA9����Ϊ��ʱ��11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           //GPIOD12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //��ʼ��PD12
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);//��ʼ����ʱ��2
	
	//��ʼ��TIM9 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2OC1

	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM9��CCR1�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM9, ENABLE);  //ʹ��TIM9
//	TIM_CtrlPWMOutputs(TIM9,ENABLE);										  
}  
void motor_break(void)
{
		INT1 = 0;
		INT2 = 0;
		INT3 = 0;
		INT4 = 0;
	  last_status = vehicle_status.runnng_status;
		vehicle_status.runnng_status=STATUS_BREAK;
}

void motor_forward(void)
{
		INT1 = 0;
		INT2 = 1;
		INT3 = 0;
		INT4 = 1;
		last_status = vehicle_status.runnng_status;
		vehicle_status.runnng_status=STATUS_ADVANCE;
}
void motor_reverse(void)
{
		INT1 = 1;
		INT2 = 0;
		INT3 = 1;
		INT4 = 0;
	  last_status = vehicle_status.runnng_status;
		vehicle_status.runnng_status=STATUS_BACKWARD;
}
void motorL_set_value(u16 value)
{
		TIM_SetCompare1(TIM9,value);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}
void motorR_set_value(u16 value)
{
		TIM_SetCompare2(TIM9,value);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}
void motor_init(void)
{
	INT_GPIO_Init();
	TIM9_1_PWM_Init(100-1,168-1);
	TIM9_2_PWM_Init(100-1,168-1);
	//motor_break();
	motor_forward();
	//motorL_set_value(70-1);
	//motorR_set_value(70-1);      

}
int Moter_PWM_Limit(int PWM)
{
	if(PWM > PWM_MAX ) PWM = PWM_MAX;
	if(PWM < PWM_MIN ) PWM = PWM_MIN;
	return  PWM;
}
int MotorR_PI(int Encoder,int Target)
{ 
	 static int Bias,Pwm,Last_bias,Last_Last_bias;
	 Bias=Target-Encoder;  
	Pwm+=Kp*(Bias-Last_bias)+Ki*Bias+Kd*(Bias-2*Last_bias+Last_Last_bias);
	Last_Last_bias = Last_bias;
	 Last_bias=Bias;	  
	 Pwm = Moter_PWM_Limit(Pwm);
	 return Pwm;  
}
int MotorL_PI(int Encoder,int Target)
{ 
	 static int Bias2,Pwm2,Last_bias2,Last_Last_bias2;
	 Bias2=Target-Encoder;  
	Pwm2+=Kp*(Bias2-Last_bias2)+Ki*Bias2+Kd*(Bias2-2*Last_bias2+Last_Last_bias2);
	Last_Last_bias2 = Last_bias2;
	 Last_bias2=Bias2;	  
	 Pwm2 = Moter_PWM_Limit(Pwm2);
	 return Pwm2;  
}