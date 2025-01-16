#ifndef __TOOL_H
#define __TOOL_H

typedef struct{double x,y;}PPos;
typedef struct{double w; int r;}JPos;
typedef struct{double x,y,dis;}Tar_PPos;
typedef struct{double aw,bw; int r;}RPos;

PPos toPPos(JPos p);
double toRad(double w);
double toDuu(double w);
int numLen(double x);
double getDist(double x1,double y1,double x2,double y2);
double getWay(PPos p);

#endif
