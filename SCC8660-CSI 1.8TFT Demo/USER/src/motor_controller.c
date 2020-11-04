#include "motor_controller.h"
const int MotorKP=60;
const int MotorKI=37*0.65;
const int MotorKD=30;
const int DirKP=11.25;
const int DirKI=0;
const int DirKD=38.7;

struct MotorController LeftBackwordMotor;
struct MotorController RightBackwordMotor;
struct DirController dircontroller;

int ChangeDirWay=Camera;
int LeftBackwordMotorSpeed=0;
int RightBackwordMotorSpeed=0;//编码器所得值

void MotorInit(struct MotorController *Which,int FPWMPort,int BPWMPort,int Speed)//电机初始化
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
void MotorSetSpeed(struct MotorController *Which,int Speed)//设定电机速度
{
  Which->SetPoint=Speed;
}


int MotorGetSetSpeed(struct MotorController *Which)
{
  return Which->SetPoint;
}


void MotorErrorUpdata(struct MotorController *Which,int MeasureValue) //返回输入值
{
  Which->LastError=Which->Error;
  Which->Error=Which->SetPoint-MeasureValue;
  Which->Integral+=Which->Error;
  //加积分限
  
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


void UpdateMotorSpeed()//根据编码器号获得编码值放在中断函数中或者循环中
{
  //左后轮
  LeftBackwordMotorSpeed=Filter(LeftBackwordMotorSpeed,qtimer_quad_get(QTIMER_1,QTIMER1_TIMER0_C0));
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER0_C0);
  //右后轮
  RightBackwordMotorSpeed=Filter(RightBackwordMotorSpeed,-qtimer_quad_get(QTIMER_1,QTIMER1_TIMER2_C2));
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER2_C2);
}


void MotorErrorUpdataAll()
{
  MotorErrorUpdata(&LeftBackwordMotor,LeftBackwordMotorSpeed);
  MotorErrorUpdata(&RightBackwordMotor,RightBackwordMotorSpeed);
}


int GetMotorSpeed(int Which)
{
  UpdateMotor();
  switch(Which)
  {
    case RB:
      return RightBackwordMotorSpeed;
    case LB:
      return LeftBackwordMotorSpeed;
  }
}


void DirControllerInit(struct DirController *Dir,struct MotorController *LBMotor,
struct MotorController *RBMotor,int SetPoint,int SetSpeed)
  {
    Dir->KP=DirKP;
    Dir->KI=DirKI;
    Dir->KD=DirKD;
    Dir->RightBackwordMotor=RBMotor;
    Dir->LeftBackwordMotor=LBMotor;
    Dir->Error=0;
    Dir->LastError=0;
    Dir->Integral=0;
    Dir->SetPoint=SetPoint;
    Dir->decrement=0;
    Dir->SetSpeed=SetSpeed;
  }


void DirErrorUpdata(struct DirController *Dir,int MeasureValue,bool DiffSpeed)
{
  //若Error>0则左偏否则右偏
  Dir->LastError=Dir->Error;
  Dir->Error=Dir->SetPoint-MeasureValue;
  int temp=Dir->KP*Dir->Error+Dir->KD*(Dir->Error-Dir->LastError);
  Dir->decrement=Dir->decrement*0.6+temp*0.4;//滑动平均
  if(DiffSpeed==false||Dir->Error<10)//不进行差速
  {
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed);
    MotorSetSpeed(Dir->RightBackwordMotor,Dir->SetSpeed);
  }
  else
  {
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed+Dir->Error*1.5);
    MotorSetSpeed(Dir->LeftBackwordMotor,Dir->SetSpeed-Dir->Error*1.5);
  }
  if(Dir->decrement+SteerMid>Steer_MAX)
    SteerSetAngle(Steer_MAX);
  else if(Dir->decrement+SteerMid<Steer_MIN)
    SteerSetAngle(Steer_MIN);
  else
    SteerSetAngle(SteerMid+Dir->decrement);
  
}
void DirSetSpeed(struct DirController *Dir,int SetSpeed)
{
  Dir->SetSpeed=SetSpeed;
}

void SetSteerAngle(int Duty)
{
    if(Duty>Steer_MAX)
      pwm_duty(PWM4_MODULE2_CHA_C30,Steer_MAX);
    else if(Duty<Steer_MIN)
      pwm_duty(PWM4_MODULE2_CHA_C30,Steer_MIN);
    else
      pwm_duty(PWM4_MODULE2_CHA_C30,Duty);
}

int Filter(int PastValue,int NextValue)//限幅加滑动平均
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


