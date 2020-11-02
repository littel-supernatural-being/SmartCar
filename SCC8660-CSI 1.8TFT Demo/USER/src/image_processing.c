#include "image_processing.h"
#define Black2White  0
#define White2Black  1
#define NoJump      -1
#define MinWidthOfdLine  1
#define LostLine   -1
#define DebugDetectJump1
uint16  GrayImage[ImageRow][ImageCol];  //�Ҷ�ͼ
uint16  BinImage[ImageRow][ImageCol];  //��ֵͼ
uint16  TerImage[ImageRow][ImageCol];  //��ֵͼ
int Stop=0;
int Slop=0;
int Threshold=34; //80;
int MidLineCol=ImageCol/2; //��������λ��
int MidLine[ImageRow];//����
bool gray_image_finish_flag=false;
bool bin_image_finish_flag=false;
bool find_line_finish_flag=false;
bool ter_image_finish_flag=false;

void ImageProcessing()
{
  if(scc8660_csi_finish_flag==false)
    return;
  else
    scc8660_csi_finish_flag=false;
  
  //GetGrayImage();//�Ȼ�ûҶ�ͼ
  //Threshold=OtsuThreshold();//����㷨�����ֵ
  GetBinAndTerImageDirect(Threshold);//��ֵ��
  FindMidLine();//���ݶ�ֵͼ��������Ѱ��
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


void GetBinAndTerImageDirect(int Treshold)
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
      if(RedTemp>16&&GreenTemp>40&&BlueTemp>25)
        TerImage[Row][Col]=BLACK;
      if(GreenTemp<23&&BlueTemp>6)
        TerImage[Row][Col]=WHITE;
      else
        TerImage[Row][Col]=GREEN;
      
    }
  }
  bin_image_finish_flag=true;
  ter_image_finish_flag=true;
  
}


void FindMidLine()
{
  int Row;
  int LastMidLineCol=ImageCol/2;//��¼��������λ�÷����´β���
  for(Row=ImageRow-1;Row>=0;Row--)
  {
    int Temp=FindMidLineInRow(Row,LastMidLineCol);
    if(Temp==LostLine||(abs(Temp-LastMidLineCol)>20&&Row<ImageRow-10))//�����ߴ���,����Ĳ������ߴ���
      MidLine[Row]=LastMidLineCol;
    else//�仯��һ����Χ��
    {
      MidLine[Row]=Temp;
      LastMidLineCol=MidLine[Row]*0.2+LastMidLineCol*0.8;
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


int FindMidLineInRow(int Row,int LastMidLineCol)//LastMidLineColΪ��һ������������������Ϊ�ο�
{
  int JumpRecorder[100][3]={0};//ʮ���ߵ������㣬������,�е�
  int LineCount=0;//��������Ŀ
  int MidLineIndex=0;//������ߵ������±�
  int MinDistanceWithMidLine=160;//����һ����������ľ���
  int Col,i;//���ڵ���
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
  }  //������������Ѳ����
    
  //��ʼ�ж��Ǹ������
  if(LineCount==0)
    return LostLine;    //����
  
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


/*
Findslop���������ж��Ƿ񼴽�����Ԫ��
��Ҫ����һ��ȫ�ֱ��� slop=0
slopΪ0��ʾδ����Ԫ�أ�slopΪ2��ʾ��������Ԫ��
�ڵ�������������Ժ���Ҫ��slop��0��
*/
void FindSlop()
{
  if(ter_image_finish_flag==false)
    return;
  int i=0;
  for (i=80;i<120;i++)
    if(TerImage[i][60]==WHITE && TerImage[i+1][60]==GREEN && TerImage[i+1][60]==GREEN)
    {
      Slop=Slop+1;
      break;
    }
  for (i=80;i>0;i--)
    if(TerImage[i][60]==WHITE && TerImage[i-1][60]==GREEN && TerImage[i-1][60]==GREEN)
    {
      Slop=Slop+1;
      break;
    }      
}

/*
Findstop���������ж��Ƿ񼴽�����Ԫ��
��Ҫ����һ��ȫ�ֱ��� stop=0
stop<9��ʾδֹͣ��stop=9��ʾ��Ҫֹͣ
*/
void FindStop()
{
  int i;
  int count1=0;
  int count2=0;
  for (i=70;i<90;i++)
  {
    if(BinImage[60][i]==BLACK)
      count1=count1+1;
    if(BinImage[61][i]==BLACK)  
      count2=count2+1;
  }
  if(count1==19 && count2<19)
  Stop=Stop+1;
}
 /*
Findstart���������ж��Ƿ�ʼ����
��Ҫ����һ��ȫ�ֱ��� start=0
start=0��ʾδ��ʼ��stop=1��ʾ��ʼ
*/
void FindStart()
{
  int count=0;
  int i,j;
  for (i=0;i<120;i++)
  {
    for (j=0;j<160;j++)
    {
      if (BinImage[i][j]==BLACK)
        count=count+1;
    }
  }
  if(count<19100)
    start=1;
}

///////�ײ��㷨///////

int DetectJump(int Row,int Col)//��������
{
  if(Col<=0||Col>=ImageRow-1)
    return NoJump;
  if(Col==1)
  {
    if(  (BinImage[Row][0]!=BinImage[Row][Col+1])
       &&(BinImage[Row][Col+1]==BinImage[Row][Col+2]))
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
       &&(BinImage[Row][Col-1]==BinImage[Row][Col-2]))
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
     BinImage[Row][Col-2]==BLACK&&BinImage[Row][Col+2]==WHITE)
    return Black2White;
  
  if(BinImage[Row][Col-1]==WHITE&&BinImage[Row][Col+1]==BLACK&&
     BinImage[Row][Col-2]==WHITE&&BinImage[Row][Col+2]==BLACK)
    return White2Black;
  
  return NoJump;
}

          
int OtsuThreshold()//����㷨
{
  if(gray_image_finish_flag==false)
    return 0;
  int i,j;
  int T=0; //Otsu�㷨��ֵ
  double varValue=0; //��䷽���м�ֵ����
  double w0=0; //ǰ�����ص�����ռ����
  double w1=0; //�������ص�����ռ����
  double u0=0; //ǰ��ƽ���Ҷ�
  double u1=0; //����ƽ���Ҷ�
  double Histogram[256]={0}; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����
  double totalNum=ImageRow*ImageCol; //��������
  //����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص���
  for(i=0;i<ImageRow;i++)   //Ϊ������������û�а�rows��cols���������
    for(j=0;j<ImageCol;j++)
      Histogram[GrayImage[i][j]]++;
  
  for(int i=0;i<255;i++)
  {
    //ÿ�α���֮ǰ��ʼ��������
    w1=0;		u1=0;		w0=0;		u0=0;
    //***********����������ֵ����**************************
    for(int j=0;j<=i;j++) //�������ָ�ֵ����
    {
      w1+=Histogram[j];  //�����������ص�����
      u1+=j*Histogram[j]; //�������������ܻҶȺ�
    }
    if(w1==0) //�����������ص���Ϊ0ʱ�˳�
      continue;
    
    u1=u1/w1; //��������ƽ���Ҷ�
    w1=w1/totalNum; // �����������ص�����ռ����
    //***********����������ֵ����**************************
 
    //***********ǰ��������ֵ����**************************
    for(int k=i+1;k<255;k++)
    {
      w0+=Histogram[k];  //ǰ���������ص�����
      u0+=k*Histogram[k]; //ǰ�����������ܻҶȺ�
    }
    if(w0==0) //ǰ���������ص���Ϊ0ʱ�˳�
      break;
    u0=u0/w0; //ǰ������ƽ���Ҷ�
    w0=w0/totalNum; // ǰ���������ص�����ռ����
		//***********ǰ��������ֵ����**************************
 
		//***********��䷽�����******************************
    double varValueI=w0*w1*(u1-u0)*(u1-u0); //��ǰ��䷽�����
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