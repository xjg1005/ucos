#ifndef __MOTOR_H
#define __MOTOR_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#define INT1_Pin	GPIO_Pin_7
#define INT2_Pin	GPIO_Pin_6
#define INT3_Pin	GPIO_Pin_5
#define INT4_Pin	GPIO_Pin_15

#define INT1 PBout(7)	// DS0
#define INT2 PBout(6)	// DS1
#define INT3 PBout(5)	// DS0
#define INT4 PBout(15)	// DS1
extern void motor_init(void);
#endif
