#include "tool.h"
#include <math.h>

const double PI = 3.1415926535897932;
	
double toRad(int w){return (double)(w / 180.0 * PI);}
int toDuu(double w){return (int)(w * 180.0 / PI);}

PPos toPPos(JPos p)
{
	PPos ret;
	p.r = p.r * 5 + 20;
	ret.x = (int)round((- ((double)p.r * cos(toRad(p.w)) - 63)));
	ret.y = (int)round((62 - ((double)p.r * sin(toRad(p.w)))));
	if(ret.y > 62) ret.y = 62;
	return ret;
}

int numLen(int x)
{
	int ret = 0;
	if(x == 0) return 1;
	while(x){ret ++; x /= 10;}
	return ret;
}

double getDist(int x1,int y1,int x2,int y2){return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));}

int getWay(PPos p)
{
	p.x -= 84;
    p.y = 63 - p.y;
    return (int)(180 - atan2(p.y,p.x) / PI * 180);
}
