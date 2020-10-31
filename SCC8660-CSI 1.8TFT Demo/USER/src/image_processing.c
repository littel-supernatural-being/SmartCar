#include "image_processing.h"
#define Black2White  0
#define White2Black  1
#define NoJump      -1
#define MinWidthOfdLine  1
#define LostLine   -1
#define DebugDetectJump1
uint16  GrayImage[ImageRow][ImageCol];  //灰度图
uint16  BinImage[ImageRow][ImageCol];  //二值图
int Threshold=38; //80;
int MidLineCol=ImageCol/2; //中线所在位置
int MidLine[ImageRow];//中线
bool gray_image_finish_flag=false;
bool bin_image_finish_flag=false;
bool find_line_finish_flag=false;


void ImageProcessing()
{
  if(scc8660_csi_finish_flag==false)
    return;
  else
    scc8660_csi_finish_flag=false;
  
  //GetGrayImage();//先获得灰度图
  //Threshold=OtsuThreshold();//大津算法获得阈值
  GetBinImageDirect(Threshold);//二值化
  FindMidLine();//根据二值图进行中线寻找
}


void GetGrayImage()
{
  uint8 RedTemp=0;
  uint8 GreenTemp=0;
  uint8 BlueTemp=0;
  uint16 Row ,Col;
  int GrayTemp;
  for(Row=0;Row<ImageRow;Row++)
  {
    for(Col=0;Col<ImageCol;Col++)
    {       
      color_camera_take_point(scc8660_csi_image[0],Col,Row,SCC8660_CSI_PIC_W,&RedTemp,&GreenTemp,&BlueTemp);
      GrayTemp=(RedTemp*77+GreenTemp*150+BlueTemp*29+128)>>8;
      GrayImage[Row][Col]=GrayTemp;
    }
  }
  gray_image_finish_flag=true;
}


void GetBinImage(int Treshold)
{
  uint16 Row ,Col;
  for(Row=0;Row<ImageRow;Row++)
  {
    for(Col=0;Col<ImageCol;Col++)
    {       
      if (GrayImage[Row][Col]<Treshold)
        BinImage[Row][Col]=BLACK;
      else
        BinImage[Row][Col]=WHITE;
    }
  }
  bin_image_finish_flag=true;
}


void GetBinImageDirect(int Treshold)
{
  uint8 RedTemp=0;
  uint8 GreenTemp=0;
  uint8 BlueTemp=0;
  uint16 Row ,Col;
  int GrayTemp;
  for(Row=0;Row<ImageRow;Row++)
  {
    for(Col=0;Col<ImageCol;Col++)
    {       
      color_camera_take_point(scc8660_csi_image[0],Col,Row,SCC8660_CSI_PIC_W,&RedTemp,&GreenTemp,&BlueTemp);
      GrayTemp=(RedTemp*77+GreenTemp*150+BlueTemp*29+128)>>8;
      if (GrayTemp<Treshold)
        BinImage[Row][Col]=BLACK;
      else
        BinImage[Row][Col]=WHITE;
    }
  }
  bin_image_finish_flag=true;
}


void FindMidLine()
{
  int Row;
  int LastMidLineCol=ImageCol/2;//记录中线所在位置方便下次查找
  for(Row=ImageRow-1;Row>=0;Row--)
  {
    int Temp=FindMidLineInRow(Row,LastMidLineCol);
    if((Temp==LostLine||abs(Temp-LastMidLineCol)>20)&&Row<ImageRow-10)//做丢线处理,最靠近的不做丢线处理
      MidLine[Row]=LastMidLineCol;
    else//变化在一定范围内
    {
      MidLine[Row]=Temp;
      LastMidLineCol=MidLine[Row];
    }
#ifdef DebugDetectJump
      lcd_drawpoint(MidLine[Row],Row,BLUE);
#endif
  }
  
  MidLineCol=0;
  for (Row = ImageRowUsedStart;Row < ImageRowUsedEnd;Row++)
    MidLineCol+=MidLine[Row];
  
  MidLineCol/=(ImageRowUsedEnd-ImageRowUsedStart);  
  find_line_finish_flag=true;
}


int FindMidLineInRow(int Row,int LastMidLineCol)//LastMidLineCol为上一次中线所在列数，作为参考
{
  int JumpRecorder[10][3]={0};//十条线的左跳点，右跳点,中点
  int LineCount=0;//总线条数目
  int MidLineIndex=0;//最靠近中线的线条下标
  int MinDistanceWithMidLine=160;//离上一行中线最近的距离
  int Col,i;//用于迭代
  int JumpType=NoJump;
  for(Col=1;Col<ImageCol-1;Col++)
  {
    JumpType=DetectJump(Row,Col);
    
#ifdef DebugDetectJump
    if(JumpType==Black2White)
      lcd_drawpoint(Col,Row,RED);
    if(JumpType==White2Black)
      lcd_drawpoint(Col,Row,GREEN);
#endif
        
    if(JumpType==Black2White)
      JumpRecorder[LineCount][0]=Col;
    else if(JumpType==White2Black)
    {
      JumpRecorder[LineCount][1]=Col;
      LineCount++;
    }
  }  //该行所有线条巡检完
    
  //开始判断那个最合适
  if(LineCount==0)
    return LostLine;    //丢线
  
  for(i=0;i<LineCount;i++)
  {
    int WidthOfdLine=(JumpRecorder[i][1]-JumpRecorder[i][0]);
    if(WidthOfdLine>=MinWidthOfdLine)
    {
      JumpRecorder[i][2]=JumpRecorder[i][0]+WidthOfdLine/2;
      if(abs(JumpRecorder[i][2]-LastMidLineCol)<MinDistanceWithMidLine)
      {
        MidLineIndex=i;
        MinDistanceWithMidLine=abs(JumpRecorder[i][2]-LastMidLineCol);
      }
    }
  }
  return JumpRecorder[MidLineIndex][2];
}




///////底层算法///////

int DetectJump(int Row,int Col)//检测跳变点
{
  if(Col<=0||Col>=ImageRow-1)
    return NoJump;
  if(Col==1)
  {
    if(  (BinImage[Row][0]!=BinImage[Row][Col+1])
       &&(BinImage[Row][Col+1]==BinImage[Row][Col+2]==BinImage[Row][Col+3]))
    {
      if(BinImage[Row][0]==BLACK)
        return Black2White;
      else
        return White2Black;
    }
    else 
      return NoJump; 
  }
  
  
   if(Col==2)
  {
    if(  (BinImage[Row][0]==BinImage[Row][1])
       &&(BinImage[Row][1]!=BinImage[Row][Col+1])
       &&(BinImage[Row][Col+1]==BinImage[Row][Col+2]==BinImage[Row][Col+3]))
    {
      if(BinImage[Row][0]==BLACK)
        return Black2White;
      else
        return White2Black;
    }
    else 
      return NoJump; 
  }
  
  
  if(Col==ImageCol-2)
  {
    if(  (BinImage[Row][Col+1]!=BinImage[Row][Col-1])
       &&(BinImage[Row][Col-1]==BinImage[Row][Col-2]==BinImage[Row][Col-3]))
    {
      if(BinImage[Row][Col+1]==BLACK)
        return White2Black;
      else
        return Black2White;
    }
    else 
      return NoJump; 
  }
  
  
  if(Col==ImageCol-3)
  {
    if(  (BinImage[Row][Col+1]==BinImage[Row][Col+2])
       &&(BinImage[Row][Col+1]!=BinImage[Row][Col-1])
       &&(BinImage[Row][Col-1]==BinImage[Row][Col-2]==BinImage[Row][Col-3]))
    {
      if(BinImage[Row][Col+1]==BLACK)
        return White2Black;
      else
        return Black2White;
    }
    else 
      return NoJump; 
  }
  
  
  if(BinImage[Row][Col-1]==BLACK&&BinImage[Row][Col+1]==WHITE&&
     BinImage[Row][Col-2]==BLACK&&BinImage[Row][Col+2]==WHITE&&
     BinImage[Row][Col-3]==BLACK&&BinImage[Row][Col+3]==WHITE)
    return Black2White;
  
  if(BinImage[Row][Col-1]==WHITE&&BinImage[Row][Col+1]==BLACK&&
     BinImage[Row][Col-2]==WHITE&&BinImage[Row][Col+2]==BLACK&&
     BinImage[Row][Col-3]==WHITE&&BinImage[Row][Col+3]==BLACK)
    return White2Black;
  
  return NoJump;
}

          
int OtsuThreshold()//大津算法
{
  if(gray_image_finish_flag==false)
    return 0;
  int i,j;
  int T=0; //Otsu算法阈值
  double varValue=0; //类间方差中间值保存
  double w0=0; //前景像素点数所占比例
  double w1=0; //背景像素点数所占比例
  double u0=0; //前景平均灰度
  double u1=0; //背景平均灰度
  double Histogram[256]={0}; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数
  double totalNum=ImageRow*ImageCol; //像素总数
  //计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数
  for(i=0;i<ImageRow;i++)   //为表述清晰，并没有把rows和cols单独提出来
    for(j=0;j<ImageCol;j++)
      Histogram[GrayImage[i][j]]++;
  
  for(int i=0;i<255;i++)
  {
    //每次遍历之前初始化各变量
    w1=0;		u1=0;		w0=0;		u0=0;
    //***********背景各分量值计算**************************
    for(int j=0;j<=i;j++) //背景部分各值计算
    {
      w1+=Histogram[j];  //背景部分像素点总数
      u1+=j*Histogram[j]; //背景部分像素总灰度和
    }
    if(w1==0) //背景部分像素点数为0时退出
      continue;
    
    u1=u1/w1; //背景像素平均灰度
    w1=w1/totalNum; // 背景部分像素点数所占比例
    //***********背景各分量值计算**************************
 
    //***********前景各分量值计算**************************
    for(int k=i+1;k<255;k++)
    {
      w0+=Histogram[k];  //前景部分像素点总数
      u0+=k*Histogram[k]; //前景部分像素总灰度和
    }
    if(w0==0) //前景部分像素点数为0时退出
      break;
    u0=u0/w0; //前景像素平均灰度
    w0=w0/totalNum; // 前景部分像素点数所占比例
		//***********前景各分量值计算**************************
 
		//***********类间方差计算******************************
    double varValueI=w0*w1*(u1-u0)*(u1-u0); //当前类间方差计算
    if(varValue<varValueI)
    {
      varValue=varValueI;
      T=i;
    }
  }
  return T;
}


/*void ImageFilter(uchar* data, int width, int height)
{
  int i, j, index, sum;
  int templates[9] = { 1, 2, 1,
	               2, 4, 2,
	               1, 2, 1 };
  sum = height * width * sizeof(uchar);
  uchar *tmpdata = (uchar*)malloc(sum);
  memcpy((char*)tmpdata,(char*)data, sum);
  for(i = 1;i < height - 1;i++)
  {
    for(j = 1;j < width - 1;j++)
    {
      index = sum = 0;
      for(int m = i - 1;m < i + 2;m++)
      {
        for(int n = j - 1; n < j + 2;n++)
        {
          sum +=
20	                        tmpdata[m * width + n] *
21	                        templates[index++];
22	                }
23	            }
24	            data[i * width + j] = sum / 16;
25	        }
26	    }
27	    free(tmpdata);
28	}
}*/