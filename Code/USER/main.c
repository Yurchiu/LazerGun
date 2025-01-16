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

const int getRealLen = 5; // The length of a range.
const int getRealNum = 3; // the count of a function that been called.
const int ignoreLen = 3; // The length of ignore.

const double angleTurn = 0.25; // The angle that each time it turns.
const int detectNum = 1; // The total of detect each time it finishes a turn.

const double derrorW = 6.0; // The detect error.
const double derrorWK = -0.09; // The k value of detect error.
const double derrorO = 90.0; // The original point.

const double errorW = -0.5; // The error.
const double errorWK = 0.145; // The k value of error.
const double errorO = 85.0; // The original point.

//const int enemyLen[] = {0, 22, 21, 20, 19, 20, 17, 16, 15, 0}; // The length of enemies len.
	
// End of parameters.

const int N = (int)(180 * detectNum / angleTurn + 100);

Tar_PPos enemyPos[20];
JPos rawEnemy[N];
int ifDelete[N];
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

void addShowEnemy(JPos tmp)
{
	if(tmp.r == 9) return;
	enemyNum ++;
	enemyPos[enemyNum].x = toPPos(tmp).x;
	enemyPos[enemyNum].y = toPPos(tmp).y;
	enemyPos[enemyNum].dis = getDist
	(
		enemyPos[enemyNum].x, enemyPos[enemyNum].y,
		84, 63
	);
	OLED_ShowTarget(enemyPos[enemyNum].x,enemyPos[enemyNum].y);
	OLED_Refresh();
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
			JPos tmp;
			double WL = rawEnemy[L].w, WR = rawEnemy[R].w;
			//WL += reduceLR; WR -= reduceLR;
			tmp.r = lastEnemy;
			tmp.w = (WL + WR) / 2;
			//tmp.w += (WL * 2 + enemyLen[lastEnemy]) / 4;
			addShowEnemy(tmp);
		}
		lastEnemy = rawEnemy[i].r;
		lastPos = i;
	}
	
	OLED_ShowNum(61,43,enemyNum,numLen(enemyNum),16,1);
	OLED_Refresh();
	
	#endif
	
	// Step 5: Sort the enemies.
	
	#ifdef __STEP_5
	
	enemySort();
	
	#endif
	
	// Step 6: Kill the enemies.
	
	#ifdef __STEP_6
	
	for(int i=1;i<=enemyNum;i++)
	{
		PPos tmp;
		tmp.x = enemyPos[i].x;
		tmp.y = enemyPos[i].y;
		
		OLED_ShowNum(0,0,(int)tmp.x,3,16,1);
		OLED_ShowNum(100,0,(int)tmp.y,3,16,1);
		OLED_ShowChar(0,15,'x',16,1);
		OLED_ShowChar(118,15,'y',16,1);
		OLED_Refresh();
		
		BTurn(getWay(tmp) + errorW + errorWK * (getWay(tmp) - errorO));
		delay_ms(1500);
		
		BEEP_Open();	
		delay_ms(3100);
		BEEP_Close();
		
		OLED_RemoveTarget((int)tmp.x,(int)tmp.y);
		OLED_Refresh();
	}
	
	#endif
	
	while(1) 
	{
		
	}
}
