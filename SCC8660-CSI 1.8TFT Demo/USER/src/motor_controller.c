#include “headfile.h”
#include "inc\motor_controller.h"
void MotorInit(struct MotorController *Which,int PWMPort,int Speed)//电机初始化
{
  Which->Error=0;
  Which->LastError=0;
  Which->LastLastError=0;
  Which->result=0;
  Which->SetPoint=Speed;
  Which->PWMPort=PWMPort;
  pwm_init(PWMPort , 1000, 0);
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
  Which->LastLastError=Which->LastError;
  Which->LastError=Which->Error;
  Which->Error=Which->SetPoint-MeasureValue;
  Which->result+=Which->Error*Which->KP+Which->KD*(Which->Error-Which->LastError);
  pwm_duty(Which->PWMPort,Which->result);
}
void DirControllerInit(struct DirController Dir,struct motor_controller *RFMotor,struct MotorController *RBMotor,
  struct MotorController *LFMotor,struct MotorController *LBMotor)
  {
    Dir.RightForwordMotor=RFMotor;
    Dir.RightBackwordMotor=RBMotor;
    Dir.LeftForwordMotor=LFMotor;
    Dir.LeftBackwordMotor=LBMotor;
    Dir.Error=0;
    Dir.LastError=0;
    Dir.LastLastError=0;
    Dir.SetPoint=0;
    Dir.decrement=0;
  }
void DirErrorUpdata(struct DirController Dir,int MeasureValue)
{
  Dir.LastLastError=Dir.LastError;
  Dir.LastError=Dir.Error;
  Dir.Error=Dir.SetPoint-MeasureValue;
  Dir.decrement+=Dir.KP*Dir.Error+Dir.KD*(Dir.Error-Dir.LastError);
  int LeftSpeed=MotorGetSetSpeed(Dir.LeftForwordMotor);
  MotorSetSpeed(Dir.RightForwordMotor,LeftSpeed+Dir.decrement);
  MotorSetSpeed(Dir.RightBackwordMotor,LeftSpeed+Dir.decrement);
}
void UpdateMotorSpeed()//根据编码器号获得编码值放在中断函数中或者循环中
{
  LeftForwordMotorSpeed=qtimer_quad_get(QTIMER1_TIMER0_C0);
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER0_C0);
  LeftBackwordMotorSpeed=qtimer_quad_get(QTIMER1_TIMER2_C2);
  qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER2_C2);
  RightForwordMotorSpeed=qtimer_quad_get(QTIMER2_TIMER0_C3);
  qtimer_quad_clear(QTIMER_2,QTIMER2_TIMER0_C3);
  RightBackwordMotorSpeed=qtimer_quad_get(QTIMER3_TIMER2_B18);
  qtimer_quad_clear(QTIMER_3,QTIMER3_TIMER2_B18);
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
void MotorErrorUpdataAll()
{
  MotorErrorUpdata(LeftForwordMotor,LeftForwordMotorSpeed);
  MotorErrorUpdata(LeftBackwordMotor,LeftBackwordMotorSpeed);
  MotorErrorUpdata(RightForwordMotor,RighttForwordMotorSpeed);
  MotorErrorUpdata(RightBackwordMotor,RightBackwordMotorSpeed);
}
