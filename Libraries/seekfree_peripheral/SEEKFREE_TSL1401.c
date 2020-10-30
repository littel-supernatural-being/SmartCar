/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2016,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_TSL1401.c
 * @brief      		红孩儿(线阵CCD)函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @version    		v1.0
 * @Software 		IAR 7.2 or MDK 5.17
 * @Target core		MK60DN512VLL10
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2016-02-25
 * @note	
					TSL1401接线定义：
					------------------------------------ 
					    模块管脚       	单片机管脚
					    CCD_AO	   		查看SEEKFREE_TSL1401.h文件中的AD_CHANNEL 宏定义
					    CCD_CLK	   		查看SEEKFREE_TSL1401.h文件中的CCD_CLK_PIN宏定义
					    CCD_SI	   		查看SEEKFREE_TSL1401.h文件中的CCD_SI_PIN 宏定义
					------------------------------------ 
 ********************************************************************************************************************/



#include "SEEKFREE_TSL1401.h"
#include "zf_adc.h"
#include "zf_uart.h"
#include "board.h"

#define filter_N 4 //滑动滤波窗口长度
#define threshold_detect 6 //达到多少个下降沿判定为斑马线
vuint8 filter_buffer[filter_N]={0};//滑动滤波窗口
vuint8 tsl1401_finish_flag;
vuint8 detected_flag=0;
uint16 ccd_data[128];       //CCD数据
vuint8 ccd_data_c[128];
uint16 detection[4];
uint16 threshold=600;
uint16 falling_edge_count=0;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD初始化
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:			
//-------------------------------------------------------------------------------------------------------------------
void ccd_init(void)     
{       
    adc_init(AD_CHANNEL,0X01);      
	  pit_timer_ms(TIM_3,10);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD图像发送至上位机查看图像
//  @param      uart_n			串口号
//  @param      uart_n			线性CCD数据指针
//  @return     void			
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void ccd_send_data(UARTN_enum uart_n ,uint16 *dat)
{
   uint8 i = 0;
   uart_putchar(uart_n,0x00); 
   uart_putchar(uart_n,0xff);
   uart_putchar(uart_n,0x01);
   uart_putchar(uart_n,0x00);
   
	for(i=0; i<128; i++)        
	{
		uart_putchar(uart_n, dat[i]>>8);   //发送高8位
		uart_putchar(uart_n, dat[i]&0XFF); //发送高低8位	 	
	}
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      TSL1401线阵CCD数据采集
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ccd_collect(void)
{
    uint8 i = 0;

    CCD_CLK(1);
    CCD_SI(0);
		CCD_CLK(0);
    CCD_SI(1);
    CCD_CLK(1);
    CCD_SI(0);
	
    for(i=0;i<128;i++)
    {
		CCD_CLK(0);  
		ccd_data[i] = adc_once(AD_CHANNEL, AD_RESOLUTION); 
		CCD_CLK(1); 
    }
	tsl1401_finish_flag = 1;
}

void ccd_detect(void)//edited by liujiang
{
	uint8 i = 0;
	uint16 falling_edge_count1=0;//当前数据帧的下降沿计数
	uint16 sum = 0;
	if(tsl1401_finish_flag)
	{
		//发送图像到上位机
		ccd_send_data(UART_1, ccd_data);
		
		//*****************************************************************二值化
		for(i=0;i<128;i++)
		{
			if(ccd_data[i]>threshold)
			{			
				ccd_data_c[i]=1;
			}
			else
			{
				ccd_data_c[i]=0;
			}
		}
		
		tsl1401_finish_flag = 0;	//采集完成标志位清除			
		
		falling_edge_count=0;  //用作判定的下降沿计数，先清0
		
		//*****************************************************************判定并累加下降沿
		//******用相邻两个值判定下降沿，也可以用四个，
		//******即前两个值为1，后两个值为0，实验的时
		//******候发现两个的效果好一些，但是理论上容易产生干扰
		//*****************************************************************
		for(i=0;i<126;i++)
		{
			detection[0]=ccd_data_c[0+i*2];
			detection[1]=ccd_data_c[1+i*2];
//				detection[2]=ccd_data_c[2+i*4];
//				detection[3]=ccd_data_c[3+i*4];
			if(detection[0]==1 && detection[1]==0) 
			{
				falling_edge_count1++;
			}
		}
		
		//*****************************************************************滑动滤波
		//先更新缓存
		for(i=0;i<filter_N-1;i++)
		{
			filter_buffer[i]=filter_buffer[i+1];
		}
		filter_buffer[filter_N-1]=falling_edge_count1;
		sum = 0;
		//计算滑动滤波后的值
		for(i=0;i<filter_N;i++)
		{
			sum+=filter_buffer[i];
		}
		falling_edge_count=(uint16)sum/filter_N;
		
		//*****************************************************************斑马线判定
		//*******当下降沿超过threshold_detect个时，核心板上的蓝灯亮起
		//*****************************************************************
		if(falling_edge_count>=threshold_detect) 
		{
			detected_flag=1;
			P52=0;
		}
		else 
		{
			detected_flag=0;
			P52=1;
		}
	}
}