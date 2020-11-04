/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		IAR 8.3 or MDK 5.24
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 					���߶��壺
					------------------------------------ 
						ģ��ܽ�            ��Ƭ���ܽ�
						SDA(����ͷ��RX)     �鿴SEEKFREE_SCC8660_CSI.h�ļ��е�SCC8660_CSI_COF_UART_TX�궨��
						SCL(����ͷ��TX)     �鿴SEEKFREE_SCC8660_CSI.h�ļ��е�SCC8660_CSI_COF_UART_RX�궨��
						���ж�(VSY)         �鿴SEEKFREE_SCC8660_CSI.h�ļ��е�SCC8660_CSI_VSYNC_PIN�궨��
						���ж�(HREF)        ������İ����ӣ����գ�
						�����ж�(PCLK)      �鿴SEEKFREE_SCC8660_CSI.h�ļ��е�SCC8660_CSI_PCLK_PIN�궨��
						���ݿ�(D0-D7)       B31-B24 B31��Ӧ����ͷ�ӿ�D0
					------------------------------------ 
	
					Ĭ�Ϸֱ���               160*120
					Ĭ��FPS                  50֡
 ********************************************************************************************************************/


//�����Ƽ�IO�鿴Projecct�ļ����µ�TXT�ı�



//���µĹ��̻��߹����ƶ���λ�����ִ�����²���
//��һ�� �ر��������д򿪵��ļ�
//�ڶ��� project  clean  �ȴ��·�����������
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
       
       if(GameStatus==Start)//��ʼ
       {
         MotorSetSpeed(&LeftBackwordMotor,120);
         MotorSetSpeed(&RightBackwordMotor,120);
         GameStatus=Playing;
       }
       else if(GameStatus==Playing)
       {
         
       }
       if(GameStatus==End)//��ʼ
       {
         MotorSetSpeed(&LeftBackwordMotor,0);
         MotorSetSpeed(&RightBackwordMotor,0);
       }
#endif
    }
}

void MyInit()
{
    board_init();   	//��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    systick_delay_ms(200);	//��ʱ200ms���ȴ��������������ϵ�ɹ�
    //��ʾģʽ����Ϊ3  ����ģʽ
    //��ʾģʽ��SEEKFREE_18TFT.h�ļ��ڵ�TFT_DISPLAY_DIR�궨������
    lcd_init();     	//��ʼ��TFT��Ļ
    pit_init();
    uart_init(USART_1,9600,UART1_TX_B12,UART1_RX_B13);
    pwm_init(PWM1_MODULE0_CHA_D12, 50, 30000);//��ʼ�����en��
    pwm_init(PWM4_MODULE2_CHA_C30, 50, SteerMid);
    PhotoTubeInit();
    pit_interrupt_ms(PIT_CH1,20);//��PITһ�Ŷ˿�����20ms���ж�
    lcd_showstr(0,0,"SEEKFREE SCC8660");
    lcd_showstr(0,1,"Initializing...");
    lcd_clear(WHITE);
    scc8660_csi_init();	//��ʼ������ͷ ʹ��CSI�ӿ� 
    //���ʹ�����壬һֱ����while(!uart_receive_flag)�������Ƿ�������OK?
    //���ͼ��ֻ�ɼ�һ�Σ����鳡�ź�(VSY)�Ƿ�����OK?
    InitKey();//��ʼ������
    //�����
    MotorInit(&LeftBackwordMotor,PWM1_MODULE3_CHA_D0,PWM1_MODULE3_CHB_D1,0);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    //�Һ���
    MotorInit(&RightBackwordMotor,PWM4_MODULE2_CHA_C30,PWM2_MODULE3_CHA_D2,0);//�����ʼ��
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);
    
    DirControllerInit(&dircontroller,&LeftBackwordMotor,&RightBackwordMotor,76,0);//���õ�ͼ��Ŀ�������ʼ��
    UpdateValue2Temp();
}



