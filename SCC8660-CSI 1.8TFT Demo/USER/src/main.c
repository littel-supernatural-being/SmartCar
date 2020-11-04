/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.24
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 					接线定义：
					------------------------------------ 
						模块管脚            单片机管脚
						SDA(摄像头的RX)     查看SEEKFREE_SCC8660_CSI.h文件中的SCC8660_CSI_COF_UART_TX宏定义
						SCL(摄像头的TX)     查看SEEKFREE_SCC8660_CSI.h文件中的SCC8660_CSI_COF_UART_RX宏定义
						场中断(VSY)         查看SEEKFREE_SCC8660_CSI.h文件中的SCC8660_CSI_VSYNC_PIN宏定义
						行中断(HREF)        不与核心板连接（悬空）
						像素中断(PCLK)      查看SEEKFREE_SCC8660_CSI.h文件中的SCC8660_CSI_PCLK_PIN宏定义
						数据口(D0-D7)       B31-B24 B31对应摄像头接口D0
					------------------------------------ 
	
					默认分辨率               160*120
					默认FPS                  50帧
 ********************************************************************************************************************/


//整套推荐IO查看Projecct文件夹下的TXT文本



//打开新的工程或者工程移动了位置务必执行以下操作
//第一步 关闭上面所有打开的文件
//第二步 project  clean  等待下方进度条走完
#define DebugImage1
#define TestMotor1
#include "main.h"

int GameStatus=End;

int main(void)
{
    DisableGlobalIRQ();
    MyInit();
    EnableGlobalIRQ(0);
    while(1)
    {
#ifdef TestMotor      
      pwm_duty(LeftBackwordMotor.ForwordPWMPort,8000);
      pwm_duty(LeftBackwordMotor.ForwordPWMPort,0);
      pwm_duty(LeftBackwordMotor.BackwordPWMPort,8000);
      pwm_duty(LeftBackwordMotor.BackwordPWMPort,0);
      
      pwm_duty(RightBackwordMotor.ForwordPWMPort,8000);
      pwm_duty(RightBackwordMotor.ForwordPWMPort,0);
      pwm_duty(RightBackwordMotor.BackwordPWMPort,8000);
      pwm_duty(RightBackwordMotor.BackwordPWMPort,0);
#endif
      
#ifdef DebugImage
      if(scc8660_csi_finish_flag)		
       {
         lcd_clear(WHITE);
         scc8660_csi_finish_flag = 0;
         GetBinAndTerImageDirect(Threshold);
         lcd_displayimage8660_zoom(BinImage[0],SCC8660_CSI_PIC_W,SCC8660_CSI_PIC_H,160,128);
         FindMidLine();
       }
#endif
      
#ifndef Debug
       menu();
       //supermonitor();
       
       ImageProcessing();
       
       if(GameStatus==Start)//开始
       {
         MotorSetSpeed(&LeftBackwordMotor,120);
         MotorSetSpeed(&RightBackwordMotor,120);
         GameStatus=Playing;
       }
       else if(GameStatus==Playing)
       {
         
       }
       if(GameStatus==End)//开始
       {
         MotorSetSpeed(&LeftBackwordMotor,0);
         MotorSetSpeed(&RightBackwordMotor,0);
       }
#endif
    }
}

void MyInit()
{
    board_init();   	//务必保留，本函数用于初始化MPU 时钟 调试串口
    systick_delay_ms(200);	//延时200ms，等待主板其他外设上电成功
    //显示模式设置为3  横屏模式
    //显示模式在SEEKFREE_18TFT.h文件内的TFT_DISPLAY_DIR宏定义设置
    lcd_init();     	//初始化TFT屏幕
    pit_init();
    uart_init(USART_1,9600,UART1_TX_B12,UART1_RX_B13);
    pwm_init(PWM1_MODULE0_CHA_D12, 50, 30000);//初始化舵机en端
    pwm_init(PWM4_MODULE2_CHA_C30, 50, SteerMid);
    PhotoTubeInit();
    pit_interrupt_ms(PIT_CH1,20);//用PIT一号端口设置20ms的中断
    lcd_showstr(0,0,"SEEKFREE SCC8660");
    lcd_showstr(0,1,"Initializing...");
    lcd_clear(WHITE);
    scc8660_csi_init();	//初始化摄像头 使用CSI接口 
    //如果使用主板，一直卡在while(!uart_receive_flag)，请检查是否电池连接OK?
    //如果图像只采集一次，请检查场信号(VSY)是否连接OK?
    InitKey();//初始化按键
    //左后轮
    MotorInit(&LeftBackwordMotor,PWM1_MODULE3_CHA_D0,PWM1_MODULE3_CHB_D1,0);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    //右后轮
    MotorInit(&RightBackwordMotor,PWM4_MODULE2_CHA_C30,PWM2_MODULE3_CHA_D2,0);//电机初始化
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);
    
    DirControllerInit(&dircontroller,&LeftBackwordMotor,&RightBackwordMotor,76,0);//利用的图像的控制器初始化
    UpdateValue2Temp();
}



