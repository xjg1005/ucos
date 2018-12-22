#ifndef __SERVO_H
#define __SERVO_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
extern void servo_init(void);
extern void servo_ctrl(uint16_t value);
#endif
