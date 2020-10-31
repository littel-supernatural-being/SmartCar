#ifndef IMAGE_H
#define IMAGE_H
#include "headfile.h"
#define ImageRow SCC8660_CSI_PIC_H
#define ImageCol SCC8660_CSI_PIC_W
#define ImageRowUsedStart 75   
#define ImageRowUsedEnd   SCC8660_CSI_PIC_H-1 //使用的摄像头的行数

extern uint16  GrayImage[ImageRow][ImageCol];  //灰度图
extern uint16  BinImage[ImageRow][ImageCol];  //二值图
extern int Threshold;
extern int MidLineCol; //中线所在位置
extern int MidLine[ImageRow];//中线
extern bool gray_image_finish_flag;
extern bool bin_image_finish_flag;
extern bool find_line_finish_flag;


extern void GetGrayImage();
extern void GetBinImage(int Treshold);
extern void GetBinImageDirect(int Treshold);//不经过灰度图直接二值化
extern void FindMidLine();
extern int OtsuThreshold();//大津算法
extern int FindMidLineInRow(int Row,int LastMidLineCol);
//LastMidLineCol为上一次中线所在列数，作为参考，返回中线所在列号
extern void ImageProcessing();//图像处理的集合函数
extern int DetectJump(int Row,int Col);//检测跳变点
#endif