#include "app_vehicle_contrl.h"
#include "servo.h"
OS_TCB VehicleContrl_TaskTCB;
CPU_STK VehicleContrl_TASK_STK[VehicleContrl_STK_SIZE];
OS_Q VehicleContrl_Msg;
_VEHICLE_CTL_MSG vehicle_ctl_msg;
static uint16_t servo_value = 1500-1;
static void turn_left(void)
{
	if(servo_value<2500){
		servo_value+= 10;
	}else{
		servo_value = 2500-1;
	}
	servo_ctrl(servo_value);
}
static void turn_right(void)
{
	if(servo_value>500){
		servo_value-= 10;
	}else{
		servo_value = 500-1;
	}
	servo_ctrl(servo_value);
}

void VehicleContrl_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	OS_MSG_SIZE size;
	void *msg;
	_VEHICLE_CTL_MSG *vehicle_ctrl_msg;
	while(1)
	{
				msg=OSQPend((OS_Q*			)&VehicleContrl_Msg,   
											(OS_TICK		)0,
                    (OS_OPT			)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE*	)&size,		
                    (CPU_TS*		)0,
                    (OS_ERR*		)&err);
				vehicle_ctrl_msg = (_VEHICLE_CTL_MSG *)msg;
				printf("get vc msg 0x%x\r\n",vehicle_ctrl_msg->msg_type);

				switch(vehicle_ctrl_msg->msg_type){
					case MSG_NOTIFY_TURN_LEFT:
						turn_left();
						break;
					case MSG_NOTIFY_TURN_RIGHT:
						turn_right();
						break;
					default:
						break;
				}

	}
}
