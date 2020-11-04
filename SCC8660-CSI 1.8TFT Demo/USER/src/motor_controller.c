#include "motor_controller.h"
#define ABS1
const int MotorKP=60;
const int MotorKI=37*0.65;
const int MotorKD=30;
const int DirKP=4.7;
const int DirKI=0;
const int DirKD=0;
const int MostDecrement=800;//������
struct MotorController LeftForwordMotor;
struct MotorController LeftBackwordMotor;
struct MotorController RightForwordMotor;
struct MotorController RightBackwordMotor;
struct DirController dircontroller;
int DiffSpeedWay=Camera;
int LeftForwordMotorSpeed=0;
int LeftBackwordMotorSpeed=0;
int RightForwordMotorSpeed=0;
int RightBackwordMotorSpeed=0;//����������ֵ
int ABSValue=-0;

void MotorInit(struct MotorController *Which,int FPWMPort,int BPWMPort,int Speed)//�����ʼ��
{
  Which->KP=MotorKP;
  Which->KI=MotorKI;
  Which->KD=MotorKD;
  Which->Error=0;
  Which->LastError=0;
  Which->Integral=0;
  Which->result=0;
  Which->SetPoint=Speed;
  Which->ForwordPWMPort=FPWMPort;
  Which->BackwordPWMPort=BPWMPort;
  pwm_init(FPWMPort , 1000, 0);
  pwm_init(BPWMPort , 1000, 0);
}
void MotorSetSpeed(struct MotorController *Which,int Speed)//�趨����ٶ�
{
  Which->SetPoint=Speed;
}


int MotorGetSetSpeed(struct MotorController *Which)
{
  return Which->SetPoint;
}


void MotorErrorUpdata(struct MotorController *Which,int MeasureValue) //��������ֵ
{
  Which->LastError=Which->Error;
  Which->Error=Which->SetPoint-MeasureValue;
  Which->Integral+=Which->Error;
  //�ӻ�����
  
  if(Which->Integral>2000)
    Which->Integral=2000;
  if(Which->Integral<-2000)
    Which->Integral=-2000;
      
  Which->result=Which->KP*Which->Error+Which->KI*Which->Integral+Which->KD*(Which->Error-Which->LastError);
  
  if(Which->result<=-40000)
    Which->result=-40000;
  if(Which->result>=40000)
    Which->result=40000;
  if(Which->result>0)
  {
    pwm_duty(Which->ForwordPWMPort,Which->result);
    pwm_duty(Which->BackwordPWMPort,0);
  }
  else
  {
    pwm_duty(Which->ForwordPWMPort,0);
    pwm_duty(Which->BackwordPWMPort,-Which->result);
  }
}


void UpdateMotorSpeed()//���ݱ������Ż�ñ���ֵ�����жϺ����л���ѭ����
{
  //��ǰ��
  LeftForwordMotorSpeed=Filter(LeftForwordMotorSpeed,qtimer_quad_get(QTIMER_2,QTIMER2_TIMER0_C3));
  qtimer_quad_clear(QTIMER_2,QTIMER2_TIMER0_C3);
  //�����
  LeftBackwordMotorSpeed=Filter(LeftBackwordMotorSpeed,qtimer_quad_get(QTIMER_1,QTIMER1_TIMER0_C0));
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER0_C0);
  //��ǰ��
  RightForwordMotorSpeed=Filter(RightForwordMotorSpeed,-qtimer_quad_get(QTIMER_3,QTIMER3_TIMER2_B18));
  qtimer_quad_clear(QTIMER_3,QTIMER3_TIMER2_B18);
  //�Һ���
  RightBackwordMotorSpeed=Filter(RightBackwordMotorSpeed,-qtimer_quad_get(QTIMER_1,QTIMER1_TIMER2_C2));
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER2_C2);
}


void MotorErrorUpdataAll()
{
  MotorErrorUpdata(&LeftForwordMotor,LeftForwordMotorSpeed);
  MotorErrorUpdata(&LeftBackwordMotor,LeftBackwordMotorSpeed);
  MotorErrorUpdata(&RightForwordMotor,RightForwordMotorSpeed);
  MotorErrorUpdata(&RightBackwordMotor,RightBackwordMotorSpeed);
}


int GetMotorSpeed(int Which)
{
  UpdateMotor();
  switch(Which)
  {
    case RF:
      return RightForwordMotorSpeed;
    case LF:
      return LeftForwordMotorSpeed;
    case RB:
      return RightBackwordMotorSpeed;
    case LB:
      return LeftBackwordMotorSpeed;
  }
}


void DirControllerInit(struct DirController *Dir,struct MotorController *LFMotor,struct MotorController *LBMotor,
  struct MotorController *RFMotor,struct MotorController *RBMotor,int SetPoint,int SetSpeed)
  {
    Dir->KP=DirKP;
    Dir->KI=DirKI;
    Dir->KD=DirKD;
    Dir->RightForwordMotor=RFMotor;
    Dir->RightBackwordMotor=RBMotor;
    Dir->LeftForwordMotor=LFMotor;
    Dir->LeftBackwordMotor=LBMotor;
    Dir->Error=0;
    Dir->LastError=0;
    Dir->Integral=0;
    Dir->SetPoint=SetPoint;
    Dir->decrement=0;
    Dir->SetSpeed=SetSpeed;
  }


void DirErrorUpdata(struct DirController *Dir,int MeasureValue)
{
  Dir->LastError=Dir->Error;
  Dir->Error=Dir->SetPoint-MeasureValue;
  int temp=Dir->KP*Dir->Error+Dir->KD*(Dir->Error-Dir->LastError);
  Dir->decrement=Dir->decrement*0.6+temp*0.4;//����ƽ��
  if(abs(Dir->decrement)<4*Dir->KP)//ƫ���С�򲻽��в���
  {
    MotorSetSpeed(Dir->LeftForwordMotor,Dir->SetSpeed);
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed);
    MotorSetSpeed(Dir->RightForwordMotor,Dir->SetSpeed);
    MotorSetSpeed(Dir->RightBackwordMotor,Dir->SetSpeed);
    return ;
  }
  
  if(Dir->decrement>MostDecrement)
    Dir->decrement=MostDecrement;
  if(Dir->decrement<-MostDecrement)
    Dir->decrement=-MostDecrement;
  //ƫ���������޷�
  
  if(Dir->decrement>0)//��ƫ������Ҽ���
  {
#ifdef ABS
    if(Dir->SetSpeed-Dir->decrement/2>ABSValue)//�������
    {
      MotorSetSpeed(Dir->LeftForwordMotor,Dir->SetSpeed-Dir->decrement/2);
      MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed-Dir->decrement/2);
    }
    else
    {
      MotorSetSpeed(Dir->LeftForwordMotor,ABSValue);
      MotorSetSpeed(Dir->LeftBackwordMotor,ABSValue);
    }
#endif
    
#ifndef ABS
    MotorSetSpeed(Dir->LeftForwordMotor,Dir->SetSpeed-Dir->decrement/3);
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed-Dir->decrement/3);
#endif
    MotorSetSpeed(Dir->RightForwordMotor,Dir->SetSpeed+Dir->decrement*2/3);
    MotorSetSpeed(Dir->RightBackwordMotor,Dir->SetSpeed+Dir->decrement*2/3);
  }
  if(Dir->decrement<0)//��ƫ�����
  {
#ifdef ABS
    if(Dir->SetSpeed+Dir->decrement/2>ABSValue)//�������
    {
      MotorSetSpeed(Dir->RightForwordMotor,Dir->SetSpeed+Dir->decrement/2);
      MotorSetSpeed(Dir->RightBackwordMotor,Dir->SetSpeed+Dir->decrement/2);
    }
    else
    {
      MotorSetSpeed(Dir->RightForwordMotor,ABSValue);
      MotorSetSpeed(Dir->RightBackwordMotor,ABSValue);
    }
#endif
    
#ifndef ABS
    MotorSetSpeed(Dir->RightForwordMotor,Dir->SetSpeed+Dir->decrement/3);
    MotorSetSpeed(Dir->RightBackwordMotor,Dir->SetSpeed+Dir->decrement/3);
#endif
    MotorSetSpeed(Dir->LeftForwordMotor,Dir->SetSpeed-Dir->decrement*2/3);
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed-Dir->decrement*2/3);
  }
  
}
void DirSetSpeed(struct DirController *Dir,int SetSpeed)
{
  Dir->SetSpeed=SetSpeed;
}


int Filter(int PastValue,int NextValue)//�޷��ӻ���ƽ��
{
  if (abs(PastValue)<abs(NextValue))
  {
    PastValue=NextValue;
    return PastValue;
  }
  int error=abs(PastValue)-abs(NextValue);
  if(error/abs(PastValue)>0.35)
    return PastValue;
  else
    PastValue=PastValue*0.95+NextValue*0.05;
  return PastValue;
    
}


