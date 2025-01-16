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

const int getRealLen = 51; // The length of a range.
const int getRealNum = 3; // the count of a function that been called.
const int ignoreLen = 41; // The length of ignore.

const double angleTurn = 0.25; // The angle that each time it turns.
const int detectNum = 1; // The total of detect each time it finishes a turn.

const double derrorW = 0/*-3.0*/; // The detect error.
const double derrorWK = 0/*0.1*/; // The k value of detect error.
const double derrorO = 150.0; // The original point.

const double errorW = 0/*-12*/; // The error.
const double errorWK = 0/*-0.55*/; // The k value of error.
const double errorO = 100.0; // The original point.
	
// End of parameters.

const int N = (int)(180 * detectNum / angleTurn + 100);

Tar_PPos enemyPos[20];
JPos rawEnemy[N];
RPos enemyRange[20];
int bucket[N][10];
int enemyNum = 0, rawEnemyNum = 0;

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

void addEnemy(JPos tmp, int i)
{
	if(tmp.r == 9) return;
	enemyPos[i].x = toPPos(tmp).x;
	enemyPos[i].y = toPPos(tmp).y;
	enemyPos[i].dis = getDist
	(
		enemyPos[i].x, enemyPos[i].y,
		84, 63
	);
}

void showEnemy()
{
	for(int i = enemyNum; i >= 1; i --)
	{
		OLED_ShowTarget(enemyPos[i].x,enemyPos[i].y);
		OLED_ShowNum(0,0,enemyPos[i].x,3,16,1);
		OLED_ShowNum(100,0,enemyPos[i].y,3,16,1);
		OLED_ShowChar(0,13,'x',16,1);
		OLED_ShowChar(118,13,'y',16,1);
		OLED_Refresh();
		delay_ms(100);
	}
}

int getTheMost(int l,int r)
{
	int b[10],maxR=9,maxNum=0;
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
	
	ATurn(180);
	delay_ms(1000);
	BTurn(-45);
	delay_ms(1000);
	
	#endif
	
	// Step 3: Get the raw datas of enemies.
	
	#ifdef __STEP_3
	
	for(double j = 180; j >= 0; j -= angleTurn)
	{
		double i = 180 - j;
		ATurn(j);
		for(int k=1;k<=detectNum;k++)
		{
			int r = enemyFind();
			r = transDist(r);
			
			rawEnemyNum ++;
			rawEnemy[rawEnemyNum].r = r;
			rawEnemy[rawEnemyNum].w = i + derrorW + derrorWK * (i - derrorO);
			
			OLED_ShowNum(0,0,r,numLen(r),16,1);
			OLED_Refresh();
		}
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
		if(R - L + 1 > ignoreLen && lastEnemy != 9)
		{
			enemyNum ++;
			enemyRange[enemyNum].r = lastEnemy;
			enemyRange[enemyNum].aw = rawEnemy[L].w;
			enemyRange[enemyNum].bw = rawEnemy[R].w;
		}
		lastEnemy = rawEnemy[i].r;
		lastPos = i;
	}
	
	for(int i = 1; i <= enemyNum; i ++)
	{
		JPos tmp;
		int AW = enemyRange[i].aw, BW = enemyRange[i].bw;
		int R = enemyRange[i].r;
		
		if(i > 1 && i < enemyNum && rawEnemy[i - 1].r < R && rawEnemy[i - 1].r < R)
			tmp.w = (AW + BW) / 2;
		else if(i > 1 && rawEnemy[i - 1].r < lastEnemy)
			tmp.w = /*(WL * 4 + WR) / 5*/ AW;
		else if(i < enemyNum && rawEnemy[i + 1].r < lastEnemy)
			tmp.w = /*(WL + WR * 4) / 5*/ BW;
		else
			tmp.w = (AW + BW) / 2;
		
		addEnemy(tmp, i);
	}
	
	OLED_ShowNum(61,43,enemyNum,numLen(enemyNum),16,1);
	OLED_Refresh();
	
	#endif
	
	// Step 5: Sort the enemies.
	
	#ifdef __STEP_5
	
	enemySort();
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
		OLED_ShowNum(100,0,tmp.y,3,16,1);
		OLED_ShowChar(0,13,'x',16,1);
		OLED_ShowChar(118,13,'y',16,1);
		OLED_Refresh();
		
		BTurn(getWay(tmp) + errorW + errorWK * (getWay(tmp) - errorO));
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
