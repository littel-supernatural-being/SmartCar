#ifndef IMAGE_H
#define IMAGE_H
#include "headfile.h"
extern int  Otsuplus();//��̬��ֵ
extern int GetThreshold();//����ȡ����ֵ
//��ʴ�˲���Mode 0���ذ׺�������Ŀ,Mode 1ֻ����û�а׺�����ʱ���غڰ�������
extern int Filter(int row, uint8_t Mode,uint8_t BlackThreshldNum);
extern void FilterBinImage();
extern void searchline();
#endifS