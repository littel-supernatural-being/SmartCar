#include "image_processing.h"
#define Black2White  0
#define White2Black  1
#define NoJump      -1

const int Threshold_Min=100;









/*int GetThreshold()
{
  static LastTreshold=0;
  double Threshold=Otsuplus();
  Treshold = (int)(0.7*LastTreshold + 0.3*Treshold);//低通滤波滑动平均
  if (Treshold< Treshold_Min ) 
    Treshold = Treshold_Min;
   LastTreshold = Treshold;
   return Threshold;
  
}  

void FilterBinImage()
{
  int col,row;
  for(row=SCC8660_CSI_PIC_H-1;row>0;row--)
  {
        if(row - 90 > 70)
          ImageFilter(row - 90, 0,1);//近处滤波效果加强
        else 
          ImageFilter(row - 90, 0,1);//远处滤波效果削弱
  }
}



void  searchRow(int row,int mid,int r_size)
{
  int col;
  int JumpType=NoJump;
  int JumpRecorder[10][3]={0};
  int change_col[10][3] = {0};//用于记录行跳变量及其中线
  int MinJumpIndex = 0;//可能有效白线组号
  int JumpIndex = 0;//当前记录下标
  for(col = 0;col <SCC8660_CSI_PIC_W; col++)
  {
    JumpType = DetectChange(row,col);
    if( JumpType == Black2White  )//黑白跳变
      JumpRecorder[JumpIndex][JumpType] = col;
    else if(JumpType == White2Black)//白黑跳变
    {
      JumpRecorder[JumpIndex][JumpType] = col;
      JumpRecorder[JumpIndex][2] =  (JumpRecorder[JumpIndex][0]+ JumpRecorder[JumpIndex][1])/2;//计算中线位置 
      
      
      if((JumpRecorder[JumpIndex][1]-JumpRecorder[JumpIndex][0])>r_size)
        if (abs(mid-JumpRecorder[JumpIndex][2])<(abs(mid-JumpRecorder[MinJumpIndex][2])))
          MinJumpIndex = JumpIndex;//找到距离上行中线更近的中线位置，记录在use_num中
      
      
      if(JumpIndex == 9)
        JumpIndex = 9;
      else
         JumpIndex++; //不足10组计数++
    }                                         
  }
  //参数提取
  jump_num[row] = JumpIndex;
  roadsize = (JumpRecorder[MinJumpIndex][1]-JumpRecorder[MinJumpIndex][0]>r_size)? (JumpRecorder[MinJumpIndex][1]-JumpRecorder[use_num][0])*2/3:0;//若中线不满足线宽，则认为搜索不到白线
  if(roadsize != 0)
  {
      left_col[row] = JumpRecorder[MinJumpIndex][0];
      right_col[row] = JumpRecorder[MinJumpIndex][1];
      mid_col[row] = JumpRecorder[MinJumpIndex][2];
      mid_error[row] = mid_col[row] - midnum;//偏差计算
  }
  //变道处理
  if( main_status == 2 && pre_main_status != 2)
  {
    if( lock_rightorleft == 1)//取左侧中值
    {
      if(use_num == 0) 
      {
        left_mid = 0;//看不到左线
      }
      else
      {
        left_mid = change_col[use_num-1][2];//看到左线
      }    
    }
    else if(lock_rightorleft == -1)//取右侧中值
    {
      if(use_num == 10) 
      {
        right_mid = 93;//看不到右线
      }
      else
      {
        right_mid = change_col[use_num+1][2];//看到右线
      } 
    }
  }
}*/









/////////////////底层/////////////////
int  Otsuplus()
{
  int GrayCount[256] = { 0 };//灰度计数值
  int ThresholdValue = 0; 
  int PixelCount=SCC8660_CSI_PIC_W * SCC8660_CSI_PIC_H;
  int GrayPixleCount=0;
  int PixelDiffAllAndGary=0;
  double SumOfGray=0;
  double NewSumOfGray=0;
  double NewAveGray=0;
  double FutureAveGray=0;
  int row, col, k;//用于迭代
  double Temp=0;
  double fmax = -1.0;
  for (row = 0; row < SCC8660_CSI_PIC_H; row++)
    for (col = 0; col < SCC8660_CSI_PIC_W; col++)
      GrayCount[BinImage[row][col]]++;
  for (k = 0; k <= 255; k++)
    SumOfGray += (double)k * (double)GrayCount[k];
  for (k = 0; k <= 255; k++)
  {
    GrayPixleCount += GrayCount[k];//灰度<=k的像素点数
    if (GrayPixleCount == 0)
      continue;
    PixelDiffAllAndGary = PixelCount - GrayPixleCount;//总像素点数减去当前灰度之前所有的像素点数
    if (PixelDiffAllAndGary == 0)
      break;//达到总像素点数
    NewSumOfGray += (double)k *GrayCount[k];//当前灰度之前的所有点乘以灰度
    NewAveGray = NewSumOfGray / GrayPixleCount;//当前灰度之前的灰度均值
    FutureAveGray = (SumOfGray - NewSumOfGray) / PixelDiffAllAndGary;//剩余为计算的灰度的均值
    Temp = (double)GrayPixleCount *(double)PixelDiffAllAndGary *(NewAveGray - FutureAveGray) * (NewAveGray - FutureAveGray);
    if (Temp > fmax)//取最值
    {
      fmax = Temp;
      ThresholdValue = k;
    }
  }
  return(ThresholdValue);
}


//腐蚀滤波，Mode 0返回白黑跳变数目,Mode 1只有在没有白黑跳变时返回黑白跳变数
int ImageFilter(int row, uint8_t Mode,uint8_t BlackThreshldNum)
{
  int BlackBegin = 0;
  int BlackNum = 0;//连续黑点数目
  int col;
  int jmppoint = 0;//将黑色段变为白色段的数目
  for (col = 0; col < SCC8660_CSI_PIC_W-1; col++)
  {
    if (BinImage[row][col] >= 1 && BinImage[row][col + 1] == 0)//黑点开始
    {
      BinImage[row][col] = 255;
      BlackBegin = col + 1;
      for (int j = BlackBegin + 1; j < SCC8660_CSI_PIC_W; ++j)
      {
        if (BinImage[row][j]== 0) 
          BlackNum++;
        else break;//黑线终止
      }
      if (BlackNum <= BlackThreshldNum && BlackBegin + BlackNum < SCC8660_CSI_PIC_W-1)//如果中间黑点总数小于指定数目则全部变为白点
      {
        for (int j = BlackBegin; j < BlackBegin + BlackNum; ++j)
          BinImage[row][j] = 255;
        jmppoint++;
      }
      col = col + BlackNum;
      BlackNum = 0;
    }
    else if(jmppoint!= 0||BinImage[row][col] >= 1) //已经有黑白跳变点
      BinImage[row][col] = 1;
  }
  if (jmppoint == 0 && Mode == 1)
  {
    int WhiteNum = 0;//连续白黑数目
    for (col = 0; col <SCC8660_CSI_PIC_W ; ++col)
    {
      if (BinImage[row][col] == 0 && BinImage[row][col + 1] >= 1)
      {
        BlackBegin = col + 1;
        WhiteNum++;
        for (int j = BlackBegin + 1; j < SCC8660_CSI_PIC_W; ++j)
        {
          if (BinImage[row][j] >= 1) 
            WhiteNum++;
          else 
            break;
        }
        jmppoint++;
        col = col + WhiteNum;
        WhiteNum = 0;
      }
    }
  }
  return jmppoint;
}

//跳变确定函数
int  DetectChange(int row,int col)
{
  if (col-2 < 0) 
  {
    if(BinImage[row][col] >= 1 && BinImage[row][col+1] >= 1)
       return Black2White;
    else
      return NoJump;
  }//处于边缘，不认为为跳变
  if(col+1 == SCC8660_CSI_PIC_W) 
  { 
    if(BinImage[row][col-1] >= 1 && BinImage[row][col] >= 1)
      return White2Black;
   else 
     return NoJump;
  }//处于边缘，不认为为跳变
  if(BinImage[row][col-2] == 0 && BinImage[row][col-1] == 0 
     && BinImage[row][col] >= 1 && BinImage[row][col+1] >= 1)
    return Black2White;//黑白跳变
  else if(BinImage[row][col-2] >= 1 && BinImage[row][col-1] >= 1 
          && BinImage[row][col] == 0 && BinImage[row][col+1] == 0)
    return White2Black;//白黑跳变
  return NoJump;//以上条件皆不满足，为非跳变
}