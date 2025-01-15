#ifndef __TURN_H
#define __TURN_H

#include "sys.h"
#include "stm32f4xx.h"

void tim3_Init(u32 arr,u32 psc);
void ATurn(int w);
void BTurn(int w);

#endif
