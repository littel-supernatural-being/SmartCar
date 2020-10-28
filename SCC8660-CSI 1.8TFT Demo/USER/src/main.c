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



#include "headfile.h"

void MyInit();//��ʹ�õĵ�Ԫ��ʼ��

int main(void)
{
    DisableGlobalIRQ();
    MyInit();
    
    EnableGlobalIRQ(0);
    while(1)
    {
       menu();
       if(scc8660_csi_finish_flag)		//ͼ��ɼ����
       {
         scc8660_csi_finish_flag = 0;	//����ɼ���ɱ�־λ
         //ʹ��������ʾ����������ԭʼͼ���С �Լ�������Ҫ��ʾ�Ĵ�С�Զ��������Ż��߷Ŵ���ʾ
         //������Ĭ�ϲɼ��ֱ���Ϊ160*120����ʾ�ֱ���Ϊ160*128����������ȫ��
         lcd_displayimage8660_zoom(scc8660_csi_image[0],SCC8660_CSI_PIC_W,SCC8660_CSI_PIC_H,160,128);
        }
        
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
    pit_interrupt_ms(PIT_CH1,500);//��PITһ�Ŷ˿�����0.5ms���ж�
    //�����Ļû���κ���ʾ��������Ļ����
    lcd_showstr(0,0,"SEEKFREE SCC8660");
    lcd_showstr(0,1,"Initializing...");
    scc8660_csi_init();	//��ʼ������ͷ ʹ��CSI�ӿ� 
    //���ʹ�����壬һֱ����while(!uart_receive_flag)�������Ƿ�������OK?
    //���ͼ��ֻ�ɼ�һ�Σ����鳡�ź�(VSY)�Ƿ�����OK?
    InitKey();//��ʼ������
    //��ǰ��
    MotorInit(&LeftForwordMotor,PWM1_MODULE0_CHB_D13,PWM1_MODULE0_CHA_D12,10);
    qtimer_quad_init(QTIMER_2,QTIMER2_TIMER0_C3,QTIMER2_TIMER3_C25);
    //�����
    MotorInit(&LeftBackwordMotor,PWM1_MODULE3_CHB_D1,PWM1_MODULE3_CHA_D0,10);
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
    //��ǰ��
    MotorInit(&RightForwordMotor,PWM1_MODULE1_CHB_D15,PWM1_MODULE1_CHA_D14,10);
    qtimer_quad_init(QTIMER_3,QTIMER3_TIMER2_B18,QTIMER3_TIMER3_B19);
    //�Һ���
    MotorInit(&RightBackwordMotor,PWM2_MODULE3_CHB_D3,PWM2_MODULE3_CHA_D2,10);//�����ʼ��
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER2_C2,QTIMER1_TIMER3_C24);
     DirControllerInit(&dircontroller,&LeftForwordMotor,&LeftBackwordMotor,
      &RightForwordMotor,&RightBackwordMotor);//��������ʼ��*/
}



