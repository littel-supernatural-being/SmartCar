#ifndef TUBE_H_
#define TUBE_H_
#include "headfile.h"

extern bool phototube_finish_flag;
extern int MidLineColInTube;
extern int PhotoTubeValue[8];//从低到高为从右至左
void PhotoTubeInit();
#endif