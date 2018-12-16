#include "app_config.h"
#include "led.h"
//Status任务函数
void Status_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0=0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED0=1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}
