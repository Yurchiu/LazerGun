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

const int getRealLen = 21; // The length of a range.
const int angleTurn = 2; // The angle that each time it turns.
const int redetectNum = 4; // The total of detect each time it finishes a turn.
const int getRealNum = 2; // the count of a function that been called.
const int ignoreLen = 15; // The length of ignore.
	
// End of parameters.

const int N = 180 * redetectNum / angleTurn + 10;

Tar_PPos enemyPos[20];
JPos rawEnemy[N];
int ifDelete[N];
int bucket[N][10];
int enemyNum = 0, rawEnemyNum = 0;

void testOfKilling()
{
	enemyPos[1].x=34;
	enemyPos[1].y=23;
	enemyPos[2].x=45;
	enemyPos[2].y=13;
	enemyPos[3].x=54;
	enemyPos[3].y=12;
	enemyPos[4].x=25;
	enemyPos[4].y=42;
	enemyPos[5].x=12;
	enemyPos[5].y=84;
	enemyPos[6].x=94;
	enemyPos[6].y=35;
	enemyPos[7].x=110;
	enemyPos[7].y=4;
	enemyPos[8].x=74;
	enemyPos[8].y=41;
	enemyNum=8;
	
	for(int i=1;i<=8;i++)
	{
		enemyPos[i].dis = getDist
		(
			enemyPos[i].x, enemyPos[i].y,
			83, 63
		);
		OLED_ShowTarget(enemyPos[i].x,enemyPos[i].y);
		OLED_Refresh();
	}
}

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
		83, 63
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

int main()
{
	int lastEnemy, lastPos;
	
	delay_init(168);
 	OLED_Init();//E2 SCL | E4 SDA
	BEEP_Init();//E13 Beep | E14 Lazer
	dist_Init();//B11 Echo | B13 Trig
	tim2_Init(10-1,84-1);
	tim3_Init(20000-1,84-1);//A6 A(CH1) | A7 B(CH2)
	
	// If you want to debug the function of B Turning, please enable STEP 1,5,6 and DEBUG 1, disable STEP 2-4.
	
	#define __STEP_1
	#define __STEP_2
	#define __STEP_3
	#define __STEP_4
	#define __STEP_5
	#define __STEP_6
	//#define __DEBUG_1
	
	// Step 1: Show the ground.
	
	#ifdef __STEP_1
	
	OLED_ShowGround();
	OLED_Refresh();
	
	#endif
	
	// Step 2: Initialize the turner.
	
	#ifdef __STEP_2
	
	ATurn(180);
	delay_ms(1500);
	
	#endif
	
	// Step 3: Get the raw datas of enemies.
	
	#ifdef __STEP_3
	
	for(int j=180;j>=0;j-=angleTurn)
	{
		int i = 180 - j;
		ATurn(j);
		for(int k=1;k<=redetectNum;k++)
		{
			int r = enemyFind();
			r = transDist(r);
			
			rawEnemyNum ++;
			rawEnemy[rawEnemyNum].r = r;
			rawEnemy[rawEnemyNum].w = i;
			
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
			JPos tmp;
			tmp.r = lastEnemy;
			tmp.w = (rawEnemy[L].w + rawEnemy[R].w) / 2;
			addShowEnemy(tmp);
		}
		lastEnemy = rawEnemy[i].r;
		lastPos = i;
	}
	
	OLED_ShowNum(118,0,enemyNum,numLen(enemyNum),16,1);
	OLED_Refresh();
	
	#endif
	
	// Debug 1: Kill the enemies.
	
	#ifdef __DEBUG_1
	
	testOfKilling();
	
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
		
		OLED_ShowNum(0,0,tmp.x,3,16,1);
		OLED_ShowNum(0,18,tmp.y,3,16,1);
		
		BTurn(getWay(tmp));
		delay_ms(1500);
		
		BEEP_Open();
		delay_ms(3100);
		BEEP_Close();
		
		OLED_RemoveTarget(enemyPos[i].x,enemyPos[i].y);
		OLED_Refresh();
	}
	
	#endif
	
	while(1) 
	{
		
	}
}
