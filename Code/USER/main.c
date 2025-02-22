#include "sys.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "oled.h"
#include "beep.h"
#include "tool.h"
#include "dist.h"
#include "tick.h"
#include "turn.h"
#include <string.h>

// The parameters needing to change.

const int getRealLen = 50; // The length of a range.
const int getRealNum = 4; // the count of a function that been called.
const int ignoreLen = 50; // The length of ignore.

const double angleTurn = 0.25; // The angle that each time it turns.

const int stdLen = 50; // The stdard len of enemies.
const int ignoreStd = 50; // Ignore this optmistic.

const double derrorW = -8/*3*/; // The detect error.
const double derrorWK = 0.08/*0.04*/; // The k value of detect error.
const double derrorO = 0; // The original point.

const double errorW = 0/*-6*/; // The error.
const double errorWK = 0.04; // The k value of error.
const double errorO = 180; // The original point.
	
// End of parameters.

const int N = 1000;

Tar_PPos enemyPos[N];
JPos rawEnemy[N];
RPos enemyRange[N];
int bucket[N][10];
int enemyNum = 0, renemyNum = 0, rawEnemyNum = 0;

void enemySort()
{
	for(int i=enemyNum-1;i>=1;i--)
	{
		for(int j=1;j<=i;j++)
		{
			if(enemyPos[j].dis>enemyPos[j+1].dis)
			{
				Tar_PPos tmp=enemyPos[j+1];
				enemyPos[j+1]=enemyPos[j];
				enemyPos[j]=tmp;
			}
		}
	}
}

void addEnemy(JPos tmp)
{
	enemyNum ++;
	enemyPos[enemyNum].x = toPPos(tmp).x;
	enemyPos[enemyNum].y = toPPos(tmp).y;
	enemyPos[enemyNum].r = tmp.r;
	enemyPos[enemyNum].dis = getDist
	(
		enemyPos[enemyNum].x, enemyPos[enemyNum].y,
		84, 63
	);
}

void showEnemy()
{
	for(int i = enemyNum; i >= 1; i --)
	{
		OLED_ShowTarget(enemyPos[i].x,enemyPos[i].y);
		OLED_ShowNum(0,0,enemyPos[i].x,3,16,1);
		OLED_ShowNum(106,0,enemyPos[i].y,3,16,1);
		OLED_ShowChar(0,13,'x',16,1);
		OLED_ShowChar(118,13,'y',16,1);
		OLED_Refresh();
		delay_ms(100);
	}
}

void getStdLen()
{
	int ret = enemyRange[2].bw - enemyRange[2].aw + 1;
	OLED_ShowNum(0,0,ret,numLen(ret),16,1);
	OLED_Refresh();
	
	while(1);
}

int getTheMost(int l,int r)
{
	int b[10],maxR=9,maxNum=0;
	if(l>r) return 9;
	for(int i=1;i<=9;i++) b[i]=0;
	for(int i=l;i<=r;i++) b[rawEnemy[i].r]++;
	for(int i=1;i<=9;i++)
	{
		if(b[i]>maxNum)
		{
			maxR=i;
			maxNum=b[i];
		}
	}
	return maxR;
}

void testDetectDist()
{
	while(1)
	{
		int r = enemyFind();
		r = transDist(r);
			
		OLED_ShowNum(0,0,r,1,16,1);
		OLED_Refresh();
	}
}

int main()
{
	int lastEnemy, lastPos;
	
	delay_init(168);
 	OLED_Init();//E2 SCL | E4 SDA
	BEEP_Init();//E13 Beep | E14 Lazer
	dist_Init();//B11 Echo | B13 Trig
	tim2_Init(10-1,84-1);
	tim3_Init(20000-1,84-1);//A6 A(CH1) | A7 B(CH2)
	
	#define __STEP_1
	#define __STEP_2
	#define __STEP_3
	#define __STEP_4
	#define __STEP_5
	#define __STEP_6
	
	// Step 1: Show the ground.
	
	#ifdef __STEP_1
	
	OLED_ShowGround();
	OLED_Refresh();
	
	#endif
	
	// Step 2: Initialize the turner.
	
	#ifdef __STEP_2
	
	ATurn(200);
	delay_ms(1000);
	BTurn(-45);
	delay_ms(1000);
	
	#endif
	
	// Step 3: Get the raw datas of enemies.
	
	#ifdef __STEP_3
	
	for(double j = 200; j >= -20; j -= angleTurn)
	{
		double i = 180 - j, r = transDist(enemyFind());
		ATurn(j);
			
		rawEnemyNum ++;
		rawEnemy[rawEnemyNum].r = r;
		rawEnemy[rawEnemyNum].w = i + derrorW + derrorWK * (i - derrorO) 
		                          + errorW + errorWK * (i - errorO);
		
		//OLED_ShowNum(0,0,r,1,16,1);
		//OLED_Refresh();
	}
	
	#endif
	
	// Step 4: Get the real datas of enemies.
	
	#ifdef __STEP_4

	for(int getting = 1; getting <= getRealNum; getting ++)
	{
		memset(bucket, 0, sizeof(bucket));
		for(int i = 1; i <= rawEnemyNum - getRealLen + 1; i ++)
		{
			int l = i, r = i + getRealLen - 1;
			int tmp = getTheMost(l, r);
			for(int j = l; j <= r; j ++) bucket[j][tmp]++;
		}
		for(int i = 1; i <= rawEnemyNum; i ++)
		{
			int maxR = 9, maxNum = 0;
			for(int j = 1; j <= 9; j ++)
			{
				if(bucket[i][j] > maxNum)
				{
					maxR = j;
					maxNum = bucket[i][j];
				}
			}
			rawEnemy[i].r = maxR;
		}
	}
	
	lastEnemy = rawEnemy[1].r;
	lastPos = 1;
	
	rawEnemy[rawEnemyNum + 1].r = 0;
	
	for(int i = 2; i <= rawEnemyNum + 1; i ++)
	{
		if(rawEnemy[i].r == lastEnemy) continue;
		int L = lastPos, R = i - 1;
		if(R - L + 1 > ignoreLen)
		{
			renemyNum ++;
			enemyRange[renemyNum].r = lastEnemy;
			enemyRange[renemyNum].aw = rawEnemy[L].w;
			enemyRange[renemyNum].bw = rawEnemy[R].w;
		}
		lastEnemy = rawEnemy[i].r;
		lastPos = i;
	}
	
	for(int i = 1; i <= renemyNum; i ++)
	{
		JPos tmp;
		int AW = enemyRange[i].aw, BW = enemyRange[i].bw;
		int R = enemyRange[i].r;
		int length = BW - AW + 1;
		
		if(R == 9) continue;
		
		if(i > 1 && i < renemyNum && enemyRange[i - 1].r < 9 && enemyRange[i + 1].r < 9)
			tmp.w = (AW + BW) / 2;
		else if(i > 1 && enemyRange[i - 1].r < 9 && length <= ignoreStd)
			tmp.w = BW - stdLen / 4 + (AW + BW) / 4;
		else if(i < renemyNum && enemyRange[i + 1].r < 9 && length <= ignoreStd)
			tmp.w = AW + stdLen / 4 + (AW + BW) / 4;
		else
			tmp.w = (AW + BW) / 2;
		tmp.r = R;
		
		addEnemy(tmp);
	}
	
	#endif
	
	// Step 5: Sort the enemies.
	
	#ifdef __STEP_5
	
	enemySort();
	OLED_ShowNum(61,43,enemyNum,numLen(enemyNum),16,1);
	OLED_Refresh();
	showEnemy();
	
	#endif
	
	// Step 6: Kill the enemies.
	
	#ifdef __STEP_6
	
	for(int i=1;i<=enemyNum;i++)
	{
		PPos tmp;
		tmp.x = enemyPos[i].x;
		tmp.y = enemyPos[i].y;
		
		OLED_ShowNum(0,0,tmp.x,3,16,1);
		OLED_ShowNum(104,0,tmp.y,3,16,1);
		OLED_ShowChar(0,15,'x',16,1);
		OLED_ShowChar(118,15,'y',16,1);
		OLED_Refresh();
		
		BTurn(getWay(tmp)/* + errorW + errorWK * (getWay(tmp) - errorO)*/);
		delay_ms(1500);
		
		BEEP_Open();	
		delay_ms(3100);
		BEEP_Close();
		
		OLED_RemoveTarget(tmp.x,tmp.y);
		OLED_Refresh();
	}
	
	#endif
	
	while(1) 
	{
		
	}
}
