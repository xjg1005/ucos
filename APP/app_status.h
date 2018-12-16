#ifndef _APP_STATUS_H
#define _APP_STATUS_H
#include "app_config.h"

OS_TCB StatusTaskTCB;
CPU_STK Status_TASK_STK[Status_STK_SIZE];
void Status_task(void *p_arg);
#endif 
