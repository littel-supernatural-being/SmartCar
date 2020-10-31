#include "image_processing.h"
#define Black2White  0
#define White2Black  1
#define NoJump      -1

const int Threshold_Min=100;









/*int GetThreshold()
{
  static LastTreshold=0;
  double Threshold=Otsuplus();
  Treshold = (int)(0.7*LastTreshold + 0.3*Treshold);//��ͨ�˲�����ƽ��
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
          ImageFilter(row - 90, 0,1);//�����˲�Ч����ǿ
        else 
          ImageFilter(row - 90, 0,1);//Զ���˲�Ч������
  }
}



void  searchRow(int row,int mid,int r_size)
{
  int col;
  int JumpType=NoJump;
  int JumpRecorder[10][3]={0};
  int change_col[10][3] = {0};//���ڼ�¼����������������
  int MinJumpIndex = 0;//������Ч�������
  int JumpIndex = 0;//��ǰ��¼�±�
  for(col = 0;col <SCC8660_CSI_PIC_W; col++)
  {
    JumpType = DetectChange(row,col);
    if( JumpType == Black2White  )//�ڰ�����
      JumpRecorder[JumpIndex][JumpType] = col;
    else if(JumpType == White2Black)//�׺�����
    {
      JumpRecorder[JumpIndex][JumpType] = col;
      JumpRecorder[JumpIndex][2] =  (JumpRecorder[JumpIndex][0]+ JumpRecorder[JumpIndex][1])/2;//��������λ�� 
      
      
      if((JumpRecorder[JumpIndex][1]-JumpRecorder[JumpIndex][0])>r_size)
        if (abs(mid-JumpRecorder[JumpIndex][2])<(abs(mid-JumpRecorder[MinJumpIndex][2])))
          MinJumpIndex = JumpIndex;//�ҵ������������߸���������λ�ã���¼��use_num��
      
      
      if(JumpIndex == 9)
        JumpIndex = 9;
      else
         JumpIndex++; //����10�����++
    }                                         
  }
  //������ȡ
  jump_num[row] = JumpIndex;
  roadsize = (JumpRecorder[MinJumpIndex][1]-JumpRecorder[MinJumpIndex][0]>r_size)? (JumpRecorder[MinJumpIndex][1]-JumpRecorder[use_num][0])*2/3:0;//�����߲������߿�����Ϊ������������
  if(roadsize != 0)
  {
      left_col[row] = JumpRecorder[MinJumpIndex][0];
      right_col[row] = JumpRecorder[MinJumpIndex][1];
      mid_col[row] = JumpRecorder[MinJumpIndex][2];
      mid_error[row] = mid_col[row] - midnum;//ƫ�����
  }
  //�������
  if( main_status == 2 && pre_main_status != 2)
  {
    if( lock_rightorleft == 1)//ȡ�����ֵ
    {
      if(use_num == 0) 
      {
        left_mid = 0;//����������
      }
      else
      {
        left_mid = change_col[use_num-1][2];//��������
      }    
    }
    else if(lock_rightorleft == -1)//ȡ�Ҳ���ֵ
    {
      if(use_num == 10) 
      {
        right_mid = 93;//����������
      }
      else
      {
        right_mid = change_col[use_num+1][2];//��������
      } 
    }
  }
}*/









/////////////////�ײ�/////////////////
int  Otsuplus()
{
  int GrayCount[256] = { 0 };//�Ҷȼ���ֵ
  int ThresholdValue = 0; 
  int PixelCount=SCC8660_CSI_PIC_W * SCC8660_CSI_PIC_H;
  int GrayPixleCount=0;
  int PixelDiffAllAndGary=0;
  double SumOfGray=0;
  double NewSumOfGray=0;
  double NewAveGray=0;
  double FutureAveGray=0;
  int row, col, k;//���ڵ���
  double Temp=0;
  double fmax = -1.0;
  for (row = 0; row < SCC8660_CSI_PIC_H; row++)
    for (col = 0; col < SCC8660_CSI_PIC_W; col++)
      GrayCount[BinImage[row][col]]++;
  for (k = 0; k <= 255; k++)
    SumOfGray += (double)k * (double)GrayCount[k];
  for (k = 0; k <= 255; k++)
  {
    GrayPixleCount += GrayCount[k];//�Ҷ�<=k�����ص���
    if (GrayPixleCount == 0)
      continue;
    PixelDiffAllAndGary = PixelCount - GrayPixleCount;//�����ص�����ȥ��ǰ�Ҷ�֮ǰ���е����ص���
    if (PixelDiffAllAndGary == 0)
      break;//�ﵽ�����ص���
    NewSumOfGray += (double)k *GrayCount[k];//��ǰ�Ҷ�֮ǰ�����е���ԻҶ�
    NewAveGray = NewSumOfGray / GrayPixleCount;//��ǰ�Ҷ�֮ǰ�ĻҶȾ�ֵ
    FutureAveGray = (SumOfGray - NewSumOfGray) / PixelDiffAllAndGary;//ʣ��Ϊ����ĻҶȵľ�ֵ
    Temp = (double)GrayPixleCount *(double)PixelDiffAllAndGary *(NewAveGray - FutureAveGray) * (NewAveGray - FutureAveGray);
    if (Temp > fmax)//ȡ��ֵ
    {
      fmax = Temp;
      ThresholdValue = k;
    }
  }
  return(ThresholdValue);
}


//��ʴ�˲���Mode 0���ذ׺�������Ŀ,Mode 1ֻ����û�а׺�����ʱ���غڰ�������
int ImageFilter(int row, uint8_t Mode,uint8_t BlackThreshldNum)
{
  int BlackBegin = 0;
  int BlackNum = 0;//�����ڵ���Ŀ
  int col;
  int jmppoint = 0;//����ɫ�α�Ϊ��ɫ�ε���Ŀ
  for (col = 0; col < SCC8660_CSI_PIC_W-1; col++)
  {
    if (BinImage[row][col] >= 1 && BinImage[row][col + 1] == 0)//�ڵ㿪ʼ
    {
      BinImage[row][col] = 255;
      BlackBegin = col + 1;
      for (int j = BlackBegin + 1; j < SCC8660_CSI_PIC_W; ++j)
      {
        if (BinImage[row][j]== 0) 
          BlackNum++;
        else break;//������ֹ
      }
      if (BlackNum <= BlackThreshldNum && BlackBegin + BlackNum < SCC8660_CSI_PIC_W-1)//����м�ڵ�����С��ָ����Ŀ��ȫ����Ϊ�׵�
      {
        for (int j = BlackBegin; j < BlackBegin + BlackNum; ++j)
          BinImage[row][j] = 255;
        jmppoint++;
      }
      col = col + BlackNum;
      BlackNum = 0;
    }
    else if(jmppoint!= 0||BinImage[row][col] >= 1) //�Ѿ��кڰ������
      BinImage[row][col] = 1;
  }
  if (jmppoint == 0 && Mode == 1)
  {
    int WhiteNum = 0;//�����׺���Ŀ
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

//����ȷ������
int  DetectChange(int row,int col)
{
  if (col-2 < 0) 
  {
    if(BinImage[row][col] >= 1 && BinImage[row][col+1] >= 1)
       return Black2White;
    else
      return NoJump;
  }//���ڱ�Ե������ΪΪ����
  if(col+1 == SCC8660_CSI_PIC_W) 
  { 
    if(BinImage[row][col-1] >= 1 && BinImage[row][col] >= 1)
      return White2Black;
   else 
     return NoJump;
  }//���ڱ�Ե������ΪΪ����
  if(BinImage[row][col-2] == 0 && BinImage[row][col-1] == 0 
     && BinImage[row][col] >= 1 && BinImage[row][col+1] >= 1)
    return Black2White;//�ڰ�����
  else if(BinImage[row][col-2] >= 1 && BinImage[row][col-1] >= 1 
          && BinImage[row][col] == 0 && BinImage[row][col+1] == 0)
    return White2Black;//�׺�����
  return NoJump;//���������Բ����㣬Ϊ������
}