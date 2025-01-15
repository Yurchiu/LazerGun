#ifndef __TICK_H
#define __TICK_H

#include "stm32f4xx.h"
#include "sys.h"

void tim2_Init(u32 arr, u32 psc);
void tim5_Init(u32 arr, u32 psc);
void startTick(void);
u32 getTick_10us(void);
u32 getTick_ms(void);

#endif
