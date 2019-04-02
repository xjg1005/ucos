#include "app_vehicle_contrl.h"
#include "servo.h"
#include "sr04.h"
#include "encoder.h"
#include "motor.h"
OS_TCB VehicleContrl_TaskTCB;
CPU_STK VehicleContrl_TASK_STK[VehicleContrl_STK_SIZE];
OS_Q VehicleContrl_Msg;


_VEHICLE_CTL_MSG vehicle_ctl_msg;
_VEHICLE_SATUS vehicle_status={STATUS_BREAK,0};
uint8_t last_status=0;
static uint16_t servo_value = 1350-1;

uint8_t Target_Speed=60;
OS_TMR	tmr_SR04;	//定义一个定时器
void tmsr04_callback(void *p_tmr,void *p_arg); //定时器1回调函数
OS_TMR	tmr_encoder;	//定义一个定时器
void tmencoder_callback(void *p_tmr,void *p_arg); //定时器1回调函数
uint8_t advance_break_flag=0;
void handle_disttance(uint8_t dist){
		
		if(dist<20&&dist>10){
			if(vehicle_status.runnng_status==STATUS_ADVANCE){
				advance_break_flag = 1;
			}
			motor_break();
		}else if(dist<=10){
			motor_reverse();
		}else if(dist>=20){
			advance_break_flag=0;
			motor_forward();
		}
}
static void turn_left(void)
{
	if(servo_value<1900){
		servo_value+= 5;
	}else{
		servo_value = 1900-1;
	}
	printf("turn_left vlaue = %d\r\n",servo_value);
	servo_ctrl(servo_value);
}
static void turn_right(void)
{
	if(servo_value>800){
		servo_value-= 5;
	}else{
		servo_value = 800-1;
	}
	printf("turn_right vlaue = %d\r\n",servo_value);
	servo_ctrl(servo_value);
}

void VehicleContrl_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	OS_MSG_SIZE size;
	void *msg;
	_VEHICLE_CTL_MSG *vehicle_ctrl_msg;
	_VEHICLE_SATUS *vehicle_status_tmp;
			//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr_SR04,		//定时器1
                (CPU_CHAR	*)"tmr_SR04",		//定时器名字
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )7,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmsr04_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
	OSTmrStart(&tmr_SR04,&err);

		//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr_encoder,		//定时器1
                (CPU_CHAR	*)"tmr_encoder",		//定时器名字
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )10,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmencoder_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
	OSTmrStart(&tmr_encoder,&err);

	while(1)
	{
				msg=OSQPend((OS_Q*			)&VehicleContrl_Msg,   
											(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
				vehicle_ctrl_msg = (_VEHICLE_CTL_MSG *)msg;
//				printf("vehicle_ctrl_msg->msg_type = %x\r\n",vehicle_ctrl_msg->msg_type);
				switch(vehicle_ctrl_msg->msg_type){
					case MSG_NOTIFY_TURN_LEFT:
						turn_left();
						break;
					case MSG_NOTIFY_TURN_RIGHT:
						turn_right();
						break;
					case MSG_NOTIFY_GET_DISTANCE:
						vehicle_status_tmp = (_VEHICLE_SATUS *)(vehicle_ctrl_msg->data);
						printf("distance:%d\r\n",vehicle_status_tmp->distance);
					handle_disttance(vehicle_status_tmp->distance);
						break;

					default:
						break;
				}

	}
}
void tmsr04_callback(void *p_tmr,void *p_arg)
{
	get_Diatance();
}
void tmencoder_callback(void *p_tmr,void *p_arg)
{
	int PWM1,PWM2;
	extern int Encoder_Data[];
	Caculate_Encoder();
	PWM1 =  MotorR_PI(Encoder_Data[0],Target_Speed);
	PWM2 =  MotorL_PI(Encoder_Data[1],Target_Speed);
	motorR_set_value(PWM1);	
	motorL_set_value(PWM2);
	printf("PWM1=%d\n,PWM2=%d\n",PWM1,PWM2);

}
