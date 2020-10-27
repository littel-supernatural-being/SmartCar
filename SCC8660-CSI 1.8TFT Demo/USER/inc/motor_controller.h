#ifndef MOTOR_H_
#define MOTOR_H_
#include "headfile.h"
#define RF  1
#define LF  2
#define RB  3
#define LB  4
struct MotorController
{
  double SetPoint; //速度设定值
  double KP=0;
  double KI=0;
  double KD=0;
  double Error;
  double LastError;
  double LastLastError;
  int result;
  int PwMPort;
};

struct DirController
{
  struct MotorController *RightForwordMotor;
  struct MotorController *RightBackwordMotor;
  struct MotorController *LeftForwordMotor;
  struct MotorController *LeftBackwordMotor;
  double SetPoint; //误差设定值
  double KP=0;
  double KI=0;
  double KD=0;
  double Error;
  double LastError;
  double LastLastError;
  double decrement; //减量
};
extern struct MotorController LeftForwordMotor;
extern struct MotorController LeftBackwordMotor;
extern struct MotorController RightForwordMotor;
extern struct MotorController RightBackwordMotor;
extern int LeftForwordMotorSpeed;
extern int LeftBackwordMotorSpeed;
extern int RightForwordMotorSpeed;
extern int RightBackwordMotorSpeed;//编码器所得值
extern struct DirController dircontroller;
int GetMotorSpeed(int Which);
void UpdateMotorSpeed();//根据编码器号获得编码值放在中断函数中或者循环中
void MotorInit(struct MotorController *Which,int PWMPort,int Speed);//电机初始化
void MotorSetSpeed(struct MotorController *Which,int Speed);//设定电机速度
int MotorGetSetSpeed(struct MotorController *Which);
void MotorErrorUpdata(struct MotorController *Which,int MeasureValue); //更新速度值
void DirControllerInit(struct DirController Dir,struct motor_controller *LFMotor,struct MotorController *LBMotor;
  struct MotorController *RFMotor,struct MotorController *RBMotor);//方向控制器初始化
void DirErrorUpdata(struct DirController Dir,int MeasureValue);//方向值设定
void MotorErrorUpdataAll();
#endif