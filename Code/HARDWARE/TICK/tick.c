#include "tick.h"

u32 tick_10us;

void tim2_Init(u32 arr, u32 psc)
{
	NVIC_InitTypeDef NVIC_InitTypeStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period=arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 
	
	TIM_Cmd(TIM2,ENABLE); 
	
	NVIC_InitTypeStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitTypeStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitTypeStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitTypeStructure.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitTypeStructure);
}

void startTick()
{
	tick_10us = 0;
	TIM_SetCounter(TIM2,0);
}

u32 getTick_10us(){return tick_10us;}

u32 getTick_ms(){return tick_10us / 100;}

void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		tick_10us ++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
