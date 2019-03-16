#ifndef __APP_CONFIG_H_H
#define __APP_CONFIG_H_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"
#include "os_app_hooks.h"

//UCOSIII���������ȼ��û�������ʹ��
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()


//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512

#define UART_TASK_PRIO		4
#define UART_STK_SIZE 		128
#define UART_TASK_NAME		"UART task"

#define VehicleContrl_PRIO		5
#define VehicleContrl_STK_SIZE 		128
#define VehicleContrl_TASK_NAME		"VehicleContrl task"


#endif /*__APP_CONFIG_H_H*/
