#ifndef __TOOL_H
#define __TOOL_H

typedef struct{int x,y;}PPos;
typedef struct{int w,r;}JPos;
typedef struct{int x,y; double dis;}Tar_PPos;

PPos toPPos(JPos p);
double toRad(int w);
int toDuu(double w);
int numLen(int x);
double getDist(int x1,int y1,int x2,int y2);
int getWay(PPos p);

#endif
