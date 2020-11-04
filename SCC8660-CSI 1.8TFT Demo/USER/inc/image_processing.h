#ifndef IMAGE_H
#define IMAGE_H
#include "headfile.h"
#define ImageRow SCC8660_CSI_PIC_H
#define ImageCol SCC8660_CSI_PIC_W
#define ImageRowUsedStart 60   
#define ImageRowUsedEnd   75 //ʹ�õ�����ͷ������

extern uint16  GrayImage[ImageRow][ImageCol];  //�Ҷ�ͼ
extern uint16  BinImage[ImageRow][ImageCol];  //��ֵͼ
extern int Stop;
extern int Slop;
extern int Threshold;
extern int MidLineCol; //��������λ��
extern int MidLine[ImageRow];//����
extern bool gray_image_finish_flag;
extern bool bin_image_finish_flag;
extern bool find_line_finish_flag;
extern bool ter_image_finish_flag;
extern void FindStart();
extern void FindStop();
extern void GetGrayImage();
extern void GetBinImage(int Treshold);
extern void GetBinAndTerImageDirect(int Treshold);//�������Ҷ�ͼֱ�Ӷ�ֵ��
extern void FindMidLine();
extern int OtsuThreshold();//����㷨
extern int FindMidLineInRow(int Row,int LastMidLineCol);
extern void DetectGameStatus();
//LastMidLineColΪ��һ������������������Ϊ�ο����������������к�
extern void ImageProcessing();//ͼ����ļ��Ϻ���
extern int DetectJump(int Row,int Col);//��������
#endif