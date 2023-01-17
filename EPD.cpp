#include <Arduino.h>
#include "EPD.h"

//////需要哪款取消哪款的注释

//#include "GDEY0213B74.h"
#include "GDEY027T91.h"


////////////

//GPIO设置，自由定义io口

int BUSY_Pin = 14; 
int RES_Pin = 15; 
int DC_Pin = 16; 
int CS_Pin = 13; 
int SCK_Pin = 12; 
int SDI_Pin = 11;
#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH) 

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)

#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
//定义RAM空间


#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

//GPIO初始化
void EPD::GPIO_set(void)
{
  pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   pinMode(SCK_Pin, OUTPUT);    
   pinMode(SDI_Pin, OUTPUT);   
}

//模拟spi波形
void EPD::SPI_Write(unsigned char value)
{
    unsigned char i;  
   SPI_Delay(1);
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
       SPI_Delay(1);
       if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
       SPI_Delay(1);
       driver_delay_us(1);
        EPD_W21_CLK_1; 
        SPI_Delay(1);
    }
}

//写入电子纸芯片命令
void EPD::Epaper_Write_Command(unsigned char cmd)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(cmd);
  EPD_W21_CS_1;
}

//写入电子纸芯片数据
void EPD::Epaper_Write_Data(unsigned char datas)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // data write
  SPI_Write(datas);
  EPD_W21_CS_1;
}

//读取芯片状态
void EPD::Epaper_READBUSY(void)
{
 while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}

//延时函数

void EPD::driver_delay_us(unsigned int xus)  //1us
{
  for(;xus>1;xus--);
}
void EPD::driver_delay(unsigned long xms) //1ms
{  
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
  {
        for(i=0; i<256; i++);
    }
}
void EPD::DELAY_S(unsigned int delaytime)     
{
  int i,j,k;
  for(i=0;i<delaytime;i++)
  {
    for(j=0;j<4000;j++)           
    {
      for(k=0;k<222;k++);
                
    }
  }
}
void EPD::SPI_Delay(unsigned char xrate)
{
  unsigned char i;
  while(xrate)
  {
    for(i=0;i<2;i++);
    xrate--;
  }
}

//display///////////
//刷新初始化
void EPD:: EPD_HW1_Init(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
    
}
//高刷初始化
void EPD::EPD_HW_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
  
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  
      
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0xB1);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   

  Epaper_Write_Command(0x1A); // Write to temperature register
  Epaper_Write_Data(0x64);    
  Epaper_Write_Data(0x00);  
            
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0x91);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   
}





//////////////////////////////All screen update////////////////////////////////////////////
void EPD::EPD_WhiteScreen_ALL(const unsigned char * datas)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD::EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
void EPD::EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0xFF);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();    
}
void EPD::EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}

///////////////////////////Part update//////////////////////////////////////////////
void EPD::EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i;   
  const unsigned char  *datas_flag;   
  datas_flag=datas;
  Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   datas=datas_flag;
  Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}

void EPD::EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start/8;//
  x_end=x_start+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  //Reset
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
    
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);  
//  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(pgm_read_byte(&datas[i]));
   } 
   EPD_Part_Update();

}




/////////////////////////////////Single display////////////////////////////////////////////////

void EPD::EPD_WhiteScreen_Black(void)

{
   unsigned int i,k;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<250;k++)
  {
    for(i=0;i<16;i++)
    {
      Epaper_Write_Data(0x00);
      }
  }
    EPD_Update();
}

void EPD::EPD_WhiteScreen_White(void)

{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0xff);
   }
   EPD_Update(); 
  EPD_Update();
}
void EPD::EPD_DISPLAY_TEXT(void)
{
unsigned char fen_L,fen_H,miao_L,miao_H; 

    EPD_HW1_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(gImage_1); //Refresh the picture in full screen
    delay(2000);
    
//////////////////////Partial refresh digital presentation//////////////////////////////////////  
    EPD_HW1_Init(); //Electronic paper initialization
    EPD_SetRAMValue_BaseMap(gImage_basemap);  //Partial refresh background color,Brush map is a must, please do not delete
    delay(100);
    EPD_Dis_Part(0,32,gImage_num1,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num2,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num3,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num4,32,32); //x,y,DATA,resolution 32*32
    EPD_Dis_Part(0,32,gImage_num5,32,32); //x,y,DATA,resolution 32*32
   delay(1000);   
  ////////////////////////////////////////////////////////////////////////  
      //Clean
    EPD_HW1_Init();//Electronic paper initialization
    EPD_WhiteScreen_White();  //Show all white
    EPD_DeepSleep();  //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    while(1); 

}



