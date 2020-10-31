#ifndef IMAGE_H
#define IMAGE_H
#include "headfile.h"
extern int  Otsuplus();//动态阈值
extern int GetThreshold();//计算取得阈值
//腐蚀滤波，Mode 0返回白黑跳变数目,Mode 1只有在没有白黑跳变时返回黑白跳变数
extern int Filter(int row, uint8_t Mode,uint8_t BlackThreshldNum);
extern void FilterBinImage();
extern void searchline();
#endifS