#include "beep.h"

void BEEP_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	BEEP_Close();
}

void BEEP_Open()
{
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
}
void BEEP_Close()
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_13);
	GPIO_ResetBits(GPIOE,GPIO_Pin_14);
}
