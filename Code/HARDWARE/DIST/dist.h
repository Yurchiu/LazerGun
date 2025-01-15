#ifndef __DIST_H
#define __DIST_H

#include "sys.h"
#include "stm32f4xx.h"
#include "delay.h"

#define Echo GPIO_Pin_11		//HC-SR04 Echo GPIOB11
#define Trig GPIO_Pin_13		//HC-SR04 Trig GPIOB13

void dist_Init(void);
void startGet(void);
void endGet(void);
int detectGet(void);

int transDist(int d);
int enemyFind(void);

#endif
