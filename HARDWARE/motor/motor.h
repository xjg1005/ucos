#ifndef __MOTOR_H
#define __MOTOR_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
#define INT1_Pin	GPIO_Pin_9
#define INT2_Pin	GPIO_Pin_10
#define INT3_Pin	GPIO_Pin_11
#define INT4_Pin	GPIO_Pin_12

#define INT1 PEout(9)	// DS0
#define INT2 PEout(10)	// DS1
#define INT3 PEout(11)	// DS0
#define INT4 PEout(12)	// DS1
extern void motor_init(void);
extern void motor_break(void);
extern void motor_forward(void);
extern void motor_reverse(void);
#endif
