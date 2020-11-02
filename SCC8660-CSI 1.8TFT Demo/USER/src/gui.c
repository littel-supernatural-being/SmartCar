#include "gui.h"
#define para1_1 LeftForwordMotorSpeed
#define para1_2 LeftBackwordMotorSpeed
#define para1_3 RightForwordMotorSpeed
#define para1_4 RightBackwordMotorSpeed
#define para1_5 LeftPhototube
#define para1_6 MidPhototube
#define para1_7 RightPhototube  //%menu1�в���


#define para2_1 dircontroller.KP
#define para2_2 dircontroller.KI
#define para2_3 dircontroller.KD
#define para2_4 Threshold
#define para2_5 LeftForwordMotor.SetPoint
#define para2_6 LeftForwordMotor.Error
#define para2_7 LeftForwordMotor.result//%menu2�в���


float TempValue1=0,TempValue2=0,TempValue3=0;
int TempValue4=0,TempValue5=0,TempValue6=0, TempValue7=0;
uint8			page		= 1;    /* ��ʼΪ��һҳ */
int			menuRow		= 1;    /* ��¼��ǰ�ǵڼ��� ֵ����Ϊ1��2��3��4��5  �޸��������ĳ��� */
int			keyState	= 0;    /* ���ڼ�¼����ص�ֵ */
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
int WaveForm[TFT_X_MAX];//����
double Scale=10;//���ηŴ����
void menu(); //�ܺ���
void InitKey();//��ʼ������
void dispimage(int which_image);//ͼ����ʾ����
void UpdateTemp2Value();//��ʱ��Temp->��ʵ��Value
void UpdateValue2Temp();//��ʵ��Value->��ʱ��Temp
void Menu1_Show();//��һ������ҳ�����޸���
void Menu2_Show();//�ڶ�������ҳ���޸���
void FlashValueOperate();//���ı���
void SignMove();//�Ϸ��·�
void ScopeDraw(int SetValue);//����ʾ����
void ScopeGetSampleValue(int SampleValue);//���ʾ�����Ĳ���

void menu()
{
  static bool HaveUpDatePage5=false;
  if ( clearCount == 60 )//ÿ20֡ˢ��һ��
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
	/* �޸�ÿ��������ֵ */
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
  if ( page == 5 )//����ǵ���ҳ������޸Ĳ���
  {
    if(keyState==KeyConfirm)
    {
      UpdateTemp2Value();
      return;
    }
    switch ( menuRow )      /* �ж����� */
    {                       
      case 0: 
      {
          /* û�е����� */
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
      if (scc8660_csi_finish_flag) //ͼ��ɼ����
      {    
        lcd_displayimage8660_zoom(scc8660_csi_image[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
      }
    }
    break;//չʾ����ͷ�е�ԭͼ
    
    case GRAY_IMAGE:
    {
      if (gray_image_finish_flag) //ͼ��ɼ����
      {
        lcd_displayimage8660_zoom(GrayImage[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
        //lcd_displayimage7725(GrayImage[0],SCC8660_CSI_PIC_W,SCC8660_CSI_PIC_H);
      }
    }
    break;//չʾ����ͷ�е�ԭͼ
  
    case BIN_IMAGE:
    {
      if (bin_image_finish_flag) //ͼ��ɼ����
      {    
        lcd_displayimage8660_zoom(BinImage[0], SCC8660_CSI_PIC_W, SCC8660_CSI_PIC_H, 160, 128);
      }
    } 
    break;//չʾ��ֵ��ͼ��
            
    case LINE:
    {
      if (find_line_finish_flag) //Ѱ�ߴ���
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
      if ( page == 2||page==3) //չʾͼƬ       /* �ڶ�ҳ��һ�а��ϼ� */
      {
        page = page-1;
        lcd_clear(WHITE);
      }
      else if ( page == 1 )  /* ��һҳ�������һҳ */
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
    if ( page == 1||page==2||page==3 )                                                        /* ��һҳ���¼� */
    {
      page= page+1;
      menuRow = 1;
      lcd_clear(WHITE);
    }
    else if ( menuRow == 7 )                                               /* ���һ�а��¼� */
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


int keyCheck( void )                                    /*������� */
{
  int	key_lable = KeyNone;
  int	temp1, temp2;
/* �ó��ĸ������µı�ǩλ */
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
/* ��ǩλΪ0��û�м����� */
  if ( key_lable == KeyNone )                 /* û�а������� */
  {
    if ( key_temp != 0 )                    /* ֮ǰ�а������� */
    {
      temp1= key_counter;  /*���µļ����� */
      temp2= key_temp;     /* ֮ǰ�İ����ı�� */
      key_counter= 0;
      key_temp= 0;
      if ( temp1 > 1 )      //����ֻ��һ�Σ���ֻ��⵽һ�εĻ�����Ϊû��
        return(temp2);
      else 
        return(0);
    }
    else 
      return(0);
  }
/* ��ǩλ��Ϊ0���м����� */
  else/*�а������� */
  {                           
    if (key_temp == 0 )    /* ֮ǰû�м�⵽�������� */
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
       lcd_drawpoint(MidLine[i],i,BLACK);//��75���ҿ�ʼ����������������100���ҿ�ʼ����ģ��
                                          //���ȱ����100֮ǰ
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