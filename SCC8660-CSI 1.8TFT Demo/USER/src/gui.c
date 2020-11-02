#include "gui.h"
#define para1_1 LeftForwordMotorSpeed
#define para1_2 LeftBackwordMotorSpeed
#define para1_3 RightForwordMotorSpeed
#define para1_4 RightBackwordMotorSpeed
#define para1_5 LeftPhototube
#define para1_6 MidPhototube
#define para1_7 RightPhototube  //%menu1中参数


#define para2_1 dircontroller.KP
#define para2_2 dircontroller.KI
#define para2_3 dircontroller.KD
#define para2_4 Threshold
#define para2_5 LeftForwordMotor.SetPoint
#define para2_6 LeftForwordMotor.Error
#define para2_7 LeftForwordMotor.result//%menu2中参数


float TempValue1=0,TempValue2=0,TempValue3=0;
int TempValue4=0,TempValue5=0,TempValue6=0, TempValue7=0;
uint8			page		= 1;    /* 初始为第一页 */
int			menuRow		= 1;    /* 记录当前是第几排 值可以为1、2、3、4、5  修改了排数的除外 */
int			keyState	= 0;    /* 用于记录五项开关的值 */
const unsigned char	row_pos[7]	= { 0, 1, 2, 3, 4, 5, 6};
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
int WaveForm[TFT_X_MAX];//波形
double Scale=10;//波形放大比例
void menu(); //总函数
void InitKey();//初始化按键
void dispimage(int which_image);//图像显示函数
void UpdateTemp2Value();//临时量Temp->真实量Value
void UpdateValue2Temp();//真实量Value->临时量Temp
void Menu1_Show();//第一个调参页不可修改量
void Menu2_Show();//第二个调参页可修改量
void FlashValueOperate();//更改变量
void SignMove();//上翻下翻
void ScopeDraw(int SetValue);//画出示波器
void ScopeGetSampleValue(int SampleValue);//获得示波器的采样

void menu()
{
  static bool HaveUpDatePage5=false;
  if ( clearCount == 60 )//每20帧刷新一次
  {
    clearCount = 0;
    lcd_clear(WHITE);
  }
  else
    ++clearCount;
  
  if ( page == 1)
  {
    DisplayImage(CSI_IMAGE);
    HaveUpDatePage5=false;
    //DisplayImage(GRAY_IMAGE);
  }
  if(page==2)
  {
    DisplayImage(BIN_IMAGE);
    HaveUpDatePage5=false;
  }
  if(page==3)
  {
    DisplayImage(LINE);
    HaveUpDatePage5=false;
  }
  if(page==4)
  {
    Menu1_Show();
    HaveUpDatePage5=false;
  }
  if(page==5)
  {
    if(HaveUpDatePage5==false)
    {
      UpdateValue2Temp();
      HaveUpDatePage5=true;
    }
    Menu2_Show();
  }
  
  keyState = keyCheck();
  SignMove();
  FlashValueOperate();    
}


void Menu1_Show()
{       
        lcd_showstr(0,row_pos[menuRow-1],"*");
	lcd_showstr( 20, row_pos[0], "LFS");            lcd_showint16( 100, row_pos[0], para1_1);        
	lcd_showstr( 20, row_pos[1], "LBS");            lcd_showint16( 100, row_pos[1], para1_2);         
	lcd_showstr( 20, row_pos[2], "RFS");            lcd_showint16( 100, row_pos[2], para1_3);        
	lcd_showstr( 20, row_pos[3], "RBS");            lcd_showint16( 100, row_pos[3], para1_4);         
	lcd_showstr( 20, row_pos[4], "LTube");       lcd_showint16( 100, row_pos[4], para1_5);         
        lcd_showstr( 20, row_pos[5], "MTube");          lcd_showint16( 100, row_pos[5], para1_6);         
	lcd_showstr( 20, row_pos[6], "RTube");      lcd_showint16( 100, row_pos[6], para1_7);        
}
void Menu2_Show()
{       
        lcd_showstr(0,row_pos[menuRow-1],"*");
	lcd_showstr( 20, row_pos[0], "KP");            lcd_showfloat( 100, row_pos[0], TempValue1,2,1);//lcd_showint16( 100, row_pos[0], TempValue1);         
	lcd_showstr( 20, row_pos[1], "KI");            lcd_showfloat( 100, row_pos[1], TempValue2,2,1);//lcd_showint16( 100, row_pos[1], TempValue2);       
	lcd_showstr( 20, row_pos[2], "KD");            lcd_showfloat( 100, row_pos[2], TempValue3,2,1);//lcd_showint16( 100, row_pos[2], TempValue3);        
	lcd_showstr( 20, row_pos[3], "Thre");            lcd_showint16( 100, row_pos[3], TempValue4);         
	lcd_showstr( 20, row_pos[4], "SetPoint");       lcd_showint16( 100, row_pos[4], TempValue5);         
        lcd_showstr( 20, row_pos[5], "Error");          lcd_showint16( 100, row_pos[5], TempValue6);         
	lcd_showstr( 20, row_pos[6], "Result");      lcd_showint16( 100, row_pos[6], TempValue7);         
}


void FlashValueOperate()
{
	/* 修改每个变量的值 */
  if ( keyState != KeyNone )
    lcd_clear(WHITE);
  else
    return ;
  if(page==1||page==2||page==3)
  {
    if(keyState==KeyConfirm)
    {
      if(GameStatus==Playing||GameStatus==Start)
        GameStatus=End;
      else if(GameStatus==End)
        GameStatus=Start;

    }      
    if(keyState==KeyLeft)
      Threshold+=2;
    if(keyState==KeyRight)
      Threshold-=2;
  }
  if ( page == 5 )//如果是第五页则可以修改参数
  {
    if(keyState==KeyConfirm)
    {
      UpdateTemp2Value();
      return;
    }
    switch ( menuRow )      /* 判断行数 */
    {                       
      case 0: 
      {
          /* 没有第零行 */
      } 
      break;
        
      case 1: 
      {
        if ( keyState == KeyLeft )
          TempValue1-=0.1;
        else if ( keyState == KeyRight)
          TempValue1+=0.1;
      } 
      break;
      case 2: 
      {
        if ( keyState == KeyLeft )
          TempValue2-=1;
        else if ( keyState == KeyRight)
          TempValue2+=1;
      } 
      break;
                
      case 3: 
      {
        if ( keyState == KeyLeft )
          TempValue3-=0.1;
        else if ( keyState == KeyRight)
          TempValue3+=0.1;
      } 
      break;
    
      case 4: 
      {
        if ( keyState == KeyLeft )
          TempValue4-=2;
        else if ( keyState == KeyRight)
          TempValue4+=2;
      } 
      break;
                
      case 5: 
      {
        if ( keyState == KeyLeft )
          TempValue5-=20;
        else if ( keyState == KeyRight)
          TempValue5+=20;
      } 
      break;
              
      case 6: 
      {
        if ( keyState == KeyLeft )
          TempValue6--;
        else if ( keyState == KeyRight)
          TempValue6++;
      } 
      break;
              
      case 7: 
      {
        if ( keyState == KeyLeft )
          TempValue7--;
        else if ( keyState == KeyRight)
          TempValue7++;
      } 
      break;
    }
  }
}

void DisplayImage(int WhichImage)
{     
  switch(WhichImage)
  {     
    case CSI_IMAGE:
    {
      if (scc8660_csi_finish_flag) //图像采集完成
      {    
        lcd_displayimage8660_zoom(scc8660_csi_image[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
      }
    }
    break;//展示摄像头中的原图
    
    case GRAY_IMAGE:
    {
      if (gray_image_finish_flag) //图像采集完成
      {
        lcd_displayimage8660_zoom(GrayImage[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
        //lcd_displayimage7725(GrayImage[0],SCC8660_CSI_PIC_W,SCC8660_CSI_PIC_H);
      }
    }
    break;//展示摄像头中的原图
  
    case BIN_IMAGE:
    {
      if (bin_image_finish_flag) //图像采集完成
      {    
        lcd_displayimage8660_zoom(BinImage[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
      }
    } 
    break;//展示二值化图像
            
    case LINE:
    {
      if (find_line_finish_flag) //寻线代码
      {    
        lcd_clear(WHITE);
        ShowLine();
      }
    }
    break;
  }
}

void SignMove()
{

  if (keyState == KeyUp)
  {
    
    if ( menuRow == 1 )
    {
      if ( page == 2||page==3) //展示图片       /* 第二页第一行按上键 */
      {
        page = page-1;
        lcd_clear(WHITE);
      }
      else if ( page == 1 )  /* 第一页翻到最后一页 */
      {
        page = pageMax;
        lcd_clear(WHITE);
        menuRow = 7;
      }
      else 
      {
        page = page - 1;
        lcd_clear(WHITE);
        menuRow = 7;
      }
    }
    else 
      --menuRow;
  }
  else if ( keyState == KeyDown )
  {
    if ( page == 1||page==2||page==3 )                                                        /* 第一页按下键 */
    {
      page= page+1;
      menuRow = 1;
      lcd_clear(WHITE);
    }
    else if ( menuRow == 7 )                                               /* 最后一行按下键 */
    {
      if ( page == pageMax )
      {
        page	= 1;
        menuRow = 1;
        lcd_clear(WHITE);
      }
      else 
      {
	page	= page + 1;
	menuRow = 1;
	lcd_clear(WHITE);
      }
    }
    else 
    {                                                                         
      lcd_clear(WHITE);
      menuRow = menuRow + 1;
    }
  }  
}


int keyCheck( void )                                    /*按键检测 */
{
  int	key_lable = KeyNone;
  int	temp1, temp2;
/* 得出哪个键按下的标签位 */
  if (!gpio_get(C31))       
    key_lable = KeyUp;
  if (!gpio_get(C27))        
    key_lable = KeyDown;
  if (!gpio_get(C26))   
    key_lable = KeyLeft;
  if (!gpio_get(C4))    
    key_lable = KeyRight;
  if (!gpio_get(D4))
    key_lable = KeyConfirm;
/* 标签位为0，没有键按下 */
  if ( key_lable == KeyNone )                 /* 没有按键按下 */
  {
    if ( key_temp != 0 )                    /* 之前有按键按下 */
    {
      temp1= key_counter;  /*按下的计数器 */
      temp2= key_temp;     /* 之前的按键的标号 */
      key_counter= 0;
      key_temp= 0;
      if ( temp1 > 1 )      //消抖只消一次，即只检测到一次的话则认为没按
        return(temp2);
      else 
        return(0);
    }
    else 
      return(0);
  }
/* 标签位不为0，有键按下 */
  else/*有按键按下 */
  {                           
    if (key_temp == 0 )    /* 之前没有检测到按键按下 */
      key_temp = key_lable;
    key_counter++;
    return(0);
  }
}


void ShowLine()
{
  int i;
  for(int i=0;i<ImageRow;i++)
  {
    if(MidLine[i]>=0&&MidLine[i]<ImageCol)
       lcd_drawpoint(MidLine[i],i,BLACK);//从75左右开始出现清晰线条，到100左右开始出现模糊
                                          //检测缺口在100之前
  }
}


void UpdateValue2Temp()
{
     TempValue1=para2_1;
     TempValue2=para2_2;
     TempValue3=para2_3;
     TempValue4=para2_4;
     TempValue5=para2_5;
     TempValue6=para2_6;
     TempValue7=para2_7;
}

void UpdateTemp2Value()
{
    para2_1=TempValue1;
    para2_2=TempValue2;
    para2_3=TempValue3;
    para2_4=TempValue4;
    para2_5=TempValue5;
    para2_6=TempValue6;
    para2_7=TempValue7;  
}
void InitKey()
{
  gpio_init(C31,GPI,1,GPIO_PIN_CONFIG);
  gpio_init(C27,GPI,1,GPIO_PIN_CONFIG);
  gpio_init(C26,GPI,1,GPIO_PIN_CONFIG);
  gpio_init(C4, GPI,1,GPIO_PIN_CONFIG);
  gpio_init(D4, GPI,1,GPIO_PIN_CONFIG);
}









void ScopeGetSampleValue(int SampleValue)
{
  if(SampleValue<0)
    SampleValue=1;
  double temp=100;
  if(SampleValue>0)
    temp=((double)TFT_Y_MAX)/SampleValue;
  if(temp<Scale&&Scale!=0)
    Scale=temp;
  static int NextMoment=0;
  if(NextMoment==TFT_X_MAX)
  {
    for(int i=0;i<TFT_X_MAX;i++)
      WaveForm[i]=1;
    NextMoment=0;
    lcd_clear(WHITE);
  }
  else
  {   
    WaveForm[NextMoment]=SampleValue;
    NextMoment++;
  }
}
void ScopeDraw(int SetValue)
{
  lcd_clear(WHITE);
  if(SetValue<0)
    SetValue=1;
  double temp;
  if(SetValue>0)
    temp=((double)TFT_Y_MAX)/SetValue;
  if(temp<Scale&&temp!=0)
    Scale=temp;
  if(SetValue>(TFT_Y_MAX-1))
    SetValue=TFT_Y_MAX-1;
  for(int i=0;i<TFT_X_MAX;i++)
  {
    lcd_drawpoint(i,TFT_Y_MAX-1-SetValue*Scale*0.9,BLACK);
    if(TFT_Y_MAX-1-WaveForm[i]*Scale*0.9>0)
      lcd_drawpoint(i,TFT_Y_MAX-1-WaveForm[i]*Scale*0.9,BLACK);
  }
}