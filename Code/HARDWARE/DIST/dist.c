#include "dist.h"
#include "tick.h"

// The parameters needing to change.

const int Soundv_dmps = 3406; // The speed of sound.
const int distErr = 3; // The distance added to detector to correct the error.
const int distErrLen = 2; // The length of error.

// End of parameters.

void dist_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = Trig;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = Echo;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOB, Trig, Bit_RESET);
}

void startGet(){PBout(13)=1;}
void endGet(){PBout(13)=0;}
int detectGet(){return PBin(11);}

int transDist(int d)
{
	int dl,dr;
	d += distErr;
	dl = d - distErrLen;
	dr = d + distErrLen;
	if(dl <= 25 && dr >= 25) return 1;
	if(dl <= 30 && dr >= 30) return 2;
	if(dl <= 35 && dr >= 35) return 3;
	if(dl <= 40 && dr >= 40) return 4;
	if(dl <= 45 && dr >= 45) return 5;
	if(dl <= 50 && dr >= 50) return 6;
	if(dl <= 55 && dr >= 55) return 7;
	if(dl <= 60 && dr >= 60) return 8;
	return 9;
}

int enemyFind()
{
	delay_ms(10);
	startGet();
	delay_us(6);
	endGet();
	while(detectGet() == 0);
	startTick();
	while(detectGet() == 1);
	return getTick_10us() * Soundv_dmps / 20000;
}
