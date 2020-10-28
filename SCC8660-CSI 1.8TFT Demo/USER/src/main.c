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



#include "headfile.h"

void MyInit();//所使用的单元初始化

int main(void)
{
    DisableGlobalIRQ();
    MyInit();
    
    EnableGlobalIRQ(0);
    while(1)
    {
       menu();
       if(scc8660_csi_finish_flag)		//图像采集完成
       {
         scc8660_csi_finish_flag = 0;	//清除采集完成标志位
         //使用缩放显示函数，根据原始图像大小 以及设置需要显示的大小自动进行缩放或者放大显示
         //本例程默认采集分辨率为160*120，显示分辨率为160*128，纵向拉伸全屏
         lcd_displayimage8660_zoom(scc8660_csi_image[0],SCC8660_CSI_PIC_W,SCC8660_CSI_PIC_H,160,128);
        }
        
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
    pit_interrupt_ms(PIT_CH1,500);//用PIT一号端口设置0.5ms的中断
    //如果屏幕没有任何显示，请检查屏幕接线
    lcd_showstr(0,0,"SEEKFREE SCC8660");
    lcd_showstr(0,1,"Initializing...");
    scc8660_csi_init();	//初始化摄像头 使用CSI接口 
    //如果使用主板，一直卡在while(!uart_receive_flag)，请检查是否电池连接OK?
    //如果图像只采集一次，请检查场信号(VSY)是否连接OK?
    InitKey();//初始化按键
    //左前轮
    MotorInit(&LeftForwordMotor,PWM1_MODULE0_CHB_D13,PWM1_MODULE0_CHA_D12,10);
    qtimer_quad_init(QTIMER_2,QTIMER2_TIMER0_C3,QTIMER2_TIMER3_C25);
    //左后轮
    MotorInit(&LeftBackwordMotor,PWM1_MODULE3_CHB_D1,PWM1_MODULE3_CHA_D0,10);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    //右前轮
    MotorInit(&RightForwordMotor,PWM1_MODULE1_CHB_D15,PWM1_MODULE1_CHA_D14,10);
    qtimer_quad_init(QTIMER_3,QTIMER3_TIMER2_B18,QTIMER3_TIMER3_B19);
    //右后轮
    MotorInit(&RightBackwordMotor,PWM2_MODULE3_CHB_D3,PWM2_MODULE3_CHA_D2,10);//电机初始化
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);
     DirControllerInit(&dircontroller,&LeftForwordMotor,&LeftBackwordMotor,
      &RightForwordMotor,&RightBackwordMotor);//控制器初始化*/
}



