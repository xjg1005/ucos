#ifndef __APP_CONFIG_H_H
#define __APP_CONFIG_H_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"
#include "os_app_hooks.h"

//UCOSIII中以下优先级用户程序不能使用
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()


//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512

#define WIFI_TASK_PRIO		4
#define WIFI_STK_SIZE 		128
#define WIFI_TASK_NAME		"WIFI task"

#define VehicleContrl_PRIO		5
#define VehicleContrl_STK_SIZE 		128
#define VehicleContrl_TASK_NAME		"VehicleContrl task"
#endif /*__APP_CONFIG_H_H*/
