#ifndef __TURN_H
#define __TURN_H

#include "sys.h"
#include "stm32f4xx.h"

void tim3_Init(u32 arr,u32 psc);
void ATurn(double w);
void BTurn(double w);

#endif
