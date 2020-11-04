#include "phototube.h"
//光电管从out1->out8为从右至左
//对应为c8->c15
int MidLineColInTube=0;
int PhotoTubeValue[8]={0};//从低到高为从左至右
bool phototube_finish_flag=false;
void PhotoTubeInit()
{
    gpio_init(C8, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C9, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C10, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C11, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C12, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C13, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C14, GPI, 1, GPIO_PIN_CONFIG);
    gpio_init(C15, GPI, 1, GPIO_PIN_CONFIG);
}
void UpDataPhotoTube()
{
  int i=0;
  int CountOfWhite=0;//检测到白色的光电管数目
  PhotoTubeValue[7]=gpio_get(C8);//out1
  PhotoTubeValue[6]=gpio_get(C9);//out2
  PhotoTubeValue[5]=gpio_get(C10);//out3
  PhotoTubeValue[4]=gpio_get(C11);//out4
  PhotoTubeValue[3]=gpio_get(C12);//out5
  PhotoTubeValue[2]=gpio_get(C13);//out6
  PhotoTubeValue[1]=gpio_get(C14);//out7
  PhotoTubeValue[0]=gpio_get(C15);//out8
  MidLineColInTube=0;
  for(i=0;i<8;i++)
    if(PhotoTubeValue[i]==0)
    {
      MidLineColInTube+=i;
      CountOfWhite++;
    }
  if(CountOfWhite!=0)
    MidLineColInTube=MidLineColInTube*20/CountOfWhite+6;
  else
    MidLineColInTube=76;
  phototube_finish_flag=true;
}