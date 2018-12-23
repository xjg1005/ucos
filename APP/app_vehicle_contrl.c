#include "app_vehicle_contrl.h"
#include "servo.h"
#include "sr04.h"
OS_TCB VehicleContrl_TaskTCB;
CPU_STK VehicleContrl_TASK_STK[VehicleContrl_STK_SIZE];
OS_Q VehicleContrl_Msg;
_VEHICLE_CTL_MSG vehicle_ctl_msg;
_VEHICLE_CTL_MSG *vehicle_ctrl_msg;
static uint16_t servo_value = 1500-1;


OS_TMR	tmr_SR04;	//����һ����ʱ��
void tmsr04_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����

static void turn_left(void)
{
	if(servo_value<2500){
		servo_value+= 10;
	}else{
		servo_value = 2500-1;
	}
	printf("turn_left vlaue = %d\r\n",servo_value);
	servo_ctrl(servo_value);
}
static void turn_right(void)
{
	if(servo_value>500){
		servo_value-= 10;
	}else{
		servo_value = 500-1;
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
	
			//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr_SR04,		//��ʱ��1
                (CPU_CHAR	*)"tmr_SR04",		//��ʱ������
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )7,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmsr04_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	OSTmrStart(&tmr_SR04,&err);


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
						printf("distance:%d\r\n",*(vehicle_ctrl_msg->data));
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

