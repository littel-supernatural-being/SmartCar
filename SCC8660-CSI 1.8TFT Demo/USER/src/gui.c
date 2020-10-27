#include "gui.h"
#define para_1 LeftForwordMotorSpeed
#define para_2 LeftBackwordMotorSpeed
#define para_3 RightForwordMotorSpeed
#define para_4 RightBackwordMotorSpeed
#define para_5 LeftForwordMotor.SetPoint
#define para_6 LeftForwordMotor.Error
#define para_7 LeftForwordMotor.result




int  Value1=0,Value5=0, Value6=0,Value7=0;
int Value2=0,Value3=0,Value4=0;
uint8			page		= 1;    /* 初始为第一页 */
int			menuRow		= 1;    /* 记录当前是第几排 值可以为1、2、3、4、5  修改了排数的除外 */
int			keyState	= 0;    /* 用于记录五项开关的值 */
const unsigned char	row_pos[7]	= { 0, 1, 2, 3, 4, 5,6};
int			key_counter	= 0;
int			key_temp	= 0;
int			temp1;
int			temp2;
int			temp3;
int			temp4;
int			temp5;
int			temp6;
int			temp7;
int			clearCount = 0;
int                     start=0;
void menu();
void dispimage(int which_image);
void Menu1Show();
void menu()
{
	if ( clearCount == 20 )//每20帧刷新一次
	{
		clearCount = 0;
		if ( page != 1 )
		{
			lcd_clear(WHITE);
		}
	}
        else
        ++clearCount;

    
	if ( page == 1 )
	{       
          Menu1Show();
		//dispimage(CSI_IMAGE);
	}
        if(page==2)
        {
               //dispimage(BIN_IMAGE);
        }
        if(page==3)
        {
               //lcd_showuint8(0,0,road_type);
        }
        if(page==4)
        {
               //dispimage(LINE);
        }
        else if ( page == 5 )
	{
                
		//MenuShow2();
         }
}

/*void dispimage(int which_image)
{     
      switch(which_image)
      {     
            case CSI_IMAGE:
            {
                if (scc8660_csi_finish_flag) //图像采集完成
                {
                     scc8660_csi_finish_flag = 0; //清除采集完成标志位
                     lcd_displayimage8660_zoom(scc8660_csi_image[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
                }
                break;
            }//展示摄像头中的原图
            
            
            case BIN_IMAGE:
            {
                if (bin_image_finish_flag) //图像采集完成
                {
                     bin_image_finish_flag = 0; //清除采集完成标志位
                     lcd_displayimage8660_zoom(bin_image[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
                     break;
                }
            }//展示二值化图像
            
            
            
            case SRC_IMAGE:
            {
                if (src_image_finish_flag) //图像采集完成
                {
                     src_image_finish_flag = 0; //清除采集完成标志位
                     lcd_displayimage8660_zoom(src_image[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
                     break;
                }
            }//展示去噪后图像
            
            
            case LINE:
            {
                if (line_finish_flag) //寻线代码
                {
                     line_finish_flag = 0; //清除采集完成标志位
                     show_line();
                     break;
                }
            }  
      }
}*/



void Menu1Show()
{       
        lcd_showstr(0,row_pos[menuRow-1],"*");
	lcd_showstr( 20, row_pos[0], "LFS");  lcd_showint16( 100, row_pos[0], Value1);         /* OLED_ShowString_1206(72,row_pos[0],"value1",1); 显示不了浮点数，但是类型是浮点数类型，所以现实的时候还是要进行处理 */
	lcd_showstr( 20, row_pos[1], "LBS");  lcd_showint16( 100, row_pos[1], Value2);         /* OLED_ShowString_1206(72,row_pos[1],"value2",1); */
	lcd_showstr( 20, row_pos[2], "RFS");  lcd_showint16( 100, row_pos[2], Value3);         /* OLED_ShowString_1206(72,row_pos[2],"value3",1); */
	lcd_showstr( 20, row_pos[3], "RBS");  lcd_showint16( 100, row_pos[3], Value4);         /* OLED_ShowString_1206(72,row_pos[3],"value4",1); */
	lcd_showstr( 20, row_pos[4], "SetPoint");  lcd_showint16( 100, row_pos[4], Value5);         /* OLED_ShowString_1206(72,row_pos[4],"value5",1); */
        lcd_showstr( 20, row_pos[5], "Error");  lcd_showint16( 100, row_pos[5], Value6);         /* OLED_ShowString_1206(72,row_pos[2],"value3",1); */
	lcd_showstr( 20, row_pos[6], "LastError");  lcd_showint16( 100, row_pos[6], Value7);         /* OLED_ShowString_1206(72,row_pos[3],"value4",1); */
}