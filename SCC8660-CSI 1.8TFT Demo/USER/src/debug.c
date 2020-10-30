#include "headfile.h"
/*********************************************************
  @seusmartcar
  ���ſƴ���λ���ƽ�Э��
  @author��SEU_SmartCar
  @2019.10.11
  @for ֱ����
*********************************************************/
float Variable[16]={0};
float Parameter[14]={0};
char SendPara=0,send_data=0;
int testnum=1;//���Ե�ʱ���õ�


typedef union  
{  
    float fdata;  
    unsigned long ldata;  
}FloatLongType;  


//�����ĸ���������ȡ��float�ĸ����ֽ�
unsigned char BYTE0(float f)
{
	FloatLongType fl;  
  fl.fdata=f;
	return (unsigned char)fl.ldata;
}

unsigned char BYTE1(float f)
{
	FloatLongType fl;  
  fl.fdata=f;
	return (unsigned char)(fl.ldata>>8);
}
unsigned char BYTE2(float f)
{
	FloatLongType fl;  
  fl.fdata=f;
	return (unsigned char)(fl.ldata>>16);
}
unsigned char BYTE3(float f)
{
	FloatLongType fl;  
  fl.fdata=f;
	return (unsigned char)(fl.ldata>>24);
}



void my_putchar(char temp)//���ſƴ�ʹ�õĴ���
{
  uart_putchar(USART_1,temp);
}

/*����֪ͨ��λ���µ�һ�����ݿ�ʼ��Ҫ�������ݱ��뷢����*/
void Send_Begin()
{
  my_putchar(0x55);
  my_putchar(0xaa);
  my_putchar(0x11);
}


//������Ҫ���͵Ĳ���  ������16��
void Testdata_generate()
{
  static int data1;
//  static long  i;
  data1=data1+1;
  if(data1>1000) data1=0;
  Variable[0]=LeftForwordMotorSpeed;
  Variable[1]=RightForwordMotorSpeed;
  Variable[2]=3;
  Variable[3]=4;
  Variable[4]=5;
  Variable[5]=6;
  Variable[6]=7;
  Variable[7]=8;
  Variable[8]=9;
  Variable[9]=10;
  Variable[10]=11;
  Variable[11]=12;
  Variable[12]=13;
  Variable[13]=14;
  Variable[14]=15;
  Variable[15]=16;
}



//����ʵʱ����
void Send_Variable()
{
  uint8 i=0,ch=0;
  float temp=0;
  uint8 Variable_num=16;
  my_putchar(85);//Э��ͷ_1
  my_putchar(170);//Э��ͷ_2
  my_putchar(255);//Э��ͷ_3
  my_putchar(1);//Э��ͷ_4
  my_putchar(Variable_num);//���ͳ��ı�������
  for(i=0;i<Variable_num;i++)
  {
    temp=Variable[i];
    ch=BYTE0(temp);
      my_putchar(ch);
    ch=BYTE1(temp);
      my_putchar(ch);
    ch=BYTE2(temp);
      my_putchar(ch);
    ch=BYTE3(temp);
      my_putchar(ch);
  }
 my_putchar(1);//Э��β
}





//��ȡ����λ���������޸Ĵ����ڵĲ���
//����BYTE0��û��д�ã���ʱ�ò��� --2020.8.1
void Modify_Parameter(uint8 *buff)
{
//   uint8 i=0,addr=0;
//   float temp;
//   uint8 Parameter_num=14; //14���ɸĲ���
//  /*�޸Ĳ�������*/
//   for(i=0;i<Parameter_num;i++)
//  {
//       BYTE0(temp)=*(uint8*)(buff+addr);
//       addr++;
//       BYTE1(temp)=*(uint8*)(buff+addr);
//       addr++;
//       BYTE2(temp)=*(uint8*)(buff+addr);
//       addr++;
//       BYTE3(temp)=*(uint8*)(buff+addr);
//       addr++;
//       Parameter[i]=temp;
//   }
   
 /*�Ӳ��������и��²���ֵ  ʾ��*/
   //PID_P=Parameter[0];
   //PID_I=Parameter[1];
   //PID_D=Parameter[2];
}
//����14��������  �����ش�
void Send_Parameter()
{
  uint8 i=0,ch=0;
  float temp=0;
  uint8 Parameter_num=14;  //14���ɸĲ���
  
  
  /*������ֵ���µ�����������  ʾ��*/
   //Parameter[0]=PID_P;
   //Parameter[1]=PID_I;
   //Parameter[2]=PID_D;
  /*                           */
  my_putchar(85);//Э��ͷ_1
  my_putchar(170);//Э��ͷ_2
  my_putchar(255);//Э��ͷ_3
  my_putchar(2);//Э��ͷ_4
  my_putchar(Parameter_num);
  for(i=0;i<Parameter_num;i++)
  { 
    temp=Parameter[i];
    ch=BYTE0(temp);
    my_putchar(ch);
    ch=BYTE1(temp);
    my_putchar(ch);
    ch=BYTE2(temp);
    my_putchar(ch);
    ch=BYTE3(temp);
    my_putchar(ch);
  }
  my_putchar(2);////Э��β
}

//�����жϽ�����λ��14���������ݣ�Ŀǰ��δ����
void UART3_RX_IRQHandler(uint16 bytereceive)//�������ڽ������ݵ��жϺ�����bytereceiveΪ���յ�������
{
  uint16 recv=bytereceive;
  static uint8 data_cnt=0;
  static uint8 predata[10];
  static uint8 Recv_Buff[100];
  static uint8 Data_Receiving = 0;
  if(Data_Receiving)//�������ڽ���������λ���Ĳ�������
  {
    if(data_cnt<56)//4���ֽ�*14������
    {
      Recv_Buff[data_cnt]= recv;//һ������4�ֽ�
      data_cnt++;
    }
    else
    {
      data_cnt=0;    //�ﵽ֡��
      Data_Receiving=0;
      if(recv==2)  //֡β
      {
         Modify_Parameter(Recv_Buff);//��ȡ����λ���������޸Ĵ����ڵĲ���
         SendPara=1;      //�����ش�����λ����ȷ�ϲ����޸����
      }
    }
  }
  if( predata[1]==0x55&&predata[0]==0xAA)//Э��
  {
    switch(recv)         //����Э���жϹ�����
    { 
    case 1:           //��ȡ���� 
      if(SendPara==0) SendPara=1;
      break;
    case 2:           //�޸Ĳ���
      Data_Receiving=1;
      data_cnt=0;
      break;
    case 3://�������
      break;        
    case 4://���ܿ���1
      break;    
    case 5://���ܿ���2
      break;     
    case 6://���ܿ���3
      break; 
    case 7://���ܿ���4
      break;        
    default:           //
      break;
    }
  }
  predata[3]=predata[2];
  predata[2]=predata[1];
  predata[1]=predata[0];
  predata[0]=recv;
}

void supermonitor()//���ſƴ���λ��
{
  if(SendPara)
  {
    SendPara=0;
    Send_Parameter();//�����ش�����λ��
  }
  Testdata_generate();//������Ҫ���͵Ĳ���
  Send_Variable(); //���Ͳ���
  //Send_Inductor();//�����λ��������ʹ��
  //send_picture();//��У���������ò��ˣ���Ϊ���ſƴ�������ͷ��λ����bugֻ֧��80*60�ķֱ���   ʹ��ɽ����λ����
  Send_Begin();
}


void Send_Inductor()//�����λ��
{
  //�˸�
}



void send_picture()//����ͷ��λ����ֻ֧��80*60�ķֱ���    ��Ҫʹ��   ����֮���п�ħ��һ��������
{
  
}
