#include "tool.h"
#include <math.h>

const double PI = 3.1415926535897932;
	
double toRad(double w){return w / 180.0 * PI;}
double toDuu(double w){return w * 180.0 / PI;}

PPos toPPos(JPos p)
{
	PPos ret;
	p.r = p.r * 5 + 20;
	ret.x = - (p.r * cos(toRad(p.w)) - 64);
	ret.y = 62 - (p.r * sin(toRad(p.w)));
	if(ret.y > 62) ret.y = 62;
	return ret;
}

int numLen(double x)
{
	int ret = 0; x = (int)x;
	if(x == 0) return 1;
	while(x){ret ++; x /= 10;}
	return ret;
}

double getDist(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

double getWay(PPos p)
{
	p.x -= 84;
    p.y = 63 - p.y;
    return 180 - atan2(p.y,p.x) / PI * 180;
}
