#ifndef MOTOR_H_
#define MOTOR_H_
#include "headfile.h"
#define RB  1
#define LB  2
#define Tube   0
#define Camera 1
#define SteerMid 2000
#define SteerLeft 2650
#define SteerRight 1350
#define Steer_MAX SteerLeft
#define Steer_MIN SteerRight
struct MotorController
{
  double SetPoint; //速度设定值
  double KP;
  double KI;
  double KD;
  double Error;
  double LastError;
  double Integral;
  int result;
  int ForwordPWMPort;
  int BackwordPWMPort;
};

struct DirController
{
  struct MotorController *LeftBackwordMotor;
  struct MotorController *RightBackwordMotor;
  double SetPoint; //误差设定值
  double KP;
  double KI;
  double KD;
  double Error;
  double LastError;
  double Integral;
  double decrement; //减量
  int SetSpeed;//设定速度
};
extern struct MotorController LeftBackwordMotor;
extern struct MotorController RightBackwordMotor;
extern int LeftBackwordMotorSpeed;
extern int RightBackwordMotorSpeed;//编码器所得值
extern int ChangeDirWay;
extern struct DirController dircontroller;

void MotorInit(struct MotorController *Which,int FPWMPort,int BPWMPort,int Speed);//电机初始化
int GetMotorSpeed(int Which);
void UpdateMotorSpeed();//根据编码器号获得编码值放在中断函数中或者循环中
void MotorSetSpeed(struct MotorController *Which,int Speed);//设定电机速度
int MotorGetSetSpeed(struct MotorController *Which);
void DirControllerInit(struct DirController *Dir,struct MotorController *LBMotor,
  struct MotorController *RBMotor,int SetPoint,int SetSpeed);//方向控制器初始化
void DirErrorUpdata(struct DirController *Dir,int MeasureValue,bool DiffSpeed);//方向值设定
void DirSetSpeed(struct DirController *Dir,int SetSpeed);
void MotorErrorUpdataAll();
int Filter(int PastValue,int NextValue);
void SetSteerAngle(int Duty);
#endif