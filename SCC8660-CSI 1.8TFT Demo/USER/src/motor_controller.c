#include "motor_controller.h"
const int MotorKP=50;
const int MotorKI=0;
const int MotorKD=0;
const int DirKP=0;
const int DirKI=0;
const int DirKD=0;
struct MotorController LeftForwordMotor;
struct MotorController LeftBackwordMotor;
struct MotorController RightForwordMotor;
struct MotorController RightBackwordMotor;
struct DirController dircontroller;
int LeftForwordMotorSpeed=0;
int LeftBackwordMotorSpeed=0;
int RightForwordMotorSpeed=0;
int RightBackwordMotorSpeed=0;//����������ֵ
int LeftPhototube=0;
int MidPhototube=0;
int RightPhototube=0;

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
  pwm_init(FPWMPort , 5000, 0);
  pwm_init(BPWMPort , 5000, 0);
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
  Which->result=Which->KP*Which->Error+Which->KI*Which->Integral+Which->KD*(Which->Error-Which->LastError);
  if(Which->result<=-50000)
    Which->result=-49000;
  if(Which->result>=50000)
    Which->result=49000;
  if(Which->result>0)
    pwm_duty(Which->ForwordPWMPort,Which->result);
  else
    pwm_duty(Which->BackwordPWMPort,-Which->result);
}
void DirControllerInit(struct DirController *Dir,struct MotorController *LFMotor,struct MotorController *LBMotor,
  struct MotorController *RFMotor,struct MotorController *RBMotor)
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
    Dir->LastLastError=0;
    Dir->SetPoint=0;
    Dir->decrement=0;
  }
void DirErrorUpdata(struct DirController *Dir,int MeasureValue)
{
  Dir->LastLastError=Dir->LastError;
  Dir->LastError=Dir->Error;
  Dir->Error=Dir->SetPoint-MeasureValue;
  Dir->decrement+=Dir->KP*Dir->Error+Dir->KD*(Dir->Error-Dir->LastError);
  int LeftSpeed=MotorGetSetSpeed(Dir->LeftForwordMotor);//����������ٶȲ�����ұ��ٶ�
  MotorSetSpeed(Dir->RightForwordMotor,LeftSpeed+Dir->decrement);
  MotorSetSpeed(Dir->RightBackwordMotor,LeftSpeed+Dir->decrement);
}
void UpdateMotorSpeed()//���ݱ������Ż�ñ���ֵ�����жϺ����л���ѭ����
{
  //��ǰ��
  LeftForwordMotorSpeed=qtimer_quad_get(QTIMER_2,QTIMER2_TIMER0_C3);
  qtimer_quad_clear(QTIMER_2,QTIMER2_TIMER0_C3);
  //�����
  LeftBackwordMotorSpeed=-qtimer_quad_get(QTIMER_1,QTIMER1_TIMER1_C1);
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER1_C1);
  //��ǰ��
  RightForwordMotorSpeed=-qtimer_quad_get(QTIMER_3,QTIMER3_TIMER2_B18);
  qtimer_quad_clear(QTIMER_3,QTIMER3_TIMER2_B18);
  //�Һ���
  RightBackwordMotorSpeed=-qtimer_quad_get(QTIMER_3,QTIMER1_TIMER3_C24);
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER3_C24);
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
void PhototubeUpdate()
{
  LeftPhototube=adc_mean_filter(ADC_1, ADC1_CH3_B14, 10);
  MidPhototube=adc_mean_filter(ADC_1, ADC1_CH4_B15, 10);
  RightPhototube==adc_mean_filter(ADC_1, ADC1_CH10_B21, 10);
}
void MotorErrorUpdataAll()
{
  MotorErrorUpdata(&LeftForwordMotor,LeftForwordMotorSpeed);
  MotorErrorUpdata(&LeftBackwordMotor,LeftBackwordMotorSpeed);
  MotorErrorUpdata(&RightForwordMotor,RightForwordMotorSpeed);
  MotorErrorUpdata(&RightBackwordMotor,RightBackwordMotorSpeed);
}
