#include <Arduino.h>
#include "EPD.h"
#include "img.h"
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
#define MAX_LINE_BYTES 120// =960/8
#define MAX_COLUMN_BYTES  640

#define ALLSCREEN_GRAGHBYTES  76800

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
//display  ///////////
//刷新初始化SSD1680
void EPD:: EPD_HW1_Init(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(10); //At least 10ms delay  
  
  Epaper_Write_Command(0x12);       //SWRESET
  Epaper_READBUSY();         //waiting for the electronic paper IC to release the idle signal
  
  Epaper_Write_Command(0x0C);  // Soft start setting
  Epaper_Write_Data(0xAE);
  Epaper_Write_Data(0xC7);
  Epaper_Write_Data(0xC3);
  Epaper_Write_Data(0xC0);
  Epaper_Write_Data(0x80);   
  
  Epaper_Write_Command(0x01);  // Set MUX as 639
  Epaper_Write_Data(0x7F);
  Epaper_Write_Data(0x02);
  Epaper_Write_Data(0x00);
  
  Epaper_Write_Command(0x11);  // Data entry mode
  Epaper_Write_Data(0x01);
  
  Epaper_Write_Command(0x44);
  Epaper_Write_Data(0x00); // RAM x address start at 0
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0xBF); // RAM x address end at 3BFh -> 959
  Epaper_Write_Data(0x03);
  Epaper_Write_Command(0x45);
  Epaper_Write_Data(0x7F); // RAM y address start at 27Fh;
  Epaper_Write_Data(0x02);
  Epaper_Write_Data(0x00); // RAM y address end at 00h;
  Epaper_Write_Data(0x00);
  
  Epaper_Write_Command(0x3C); // VBD
  Epaper_Write_Data(0x01); // LUT1, for white
  
  Epaper_Write_Command(0x18);
  Epaper_Write_Data(0X80);
  
  Epaper_Write_Command(0x4E); 
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F); 
  Epaper_Write_Data(0x7F);
  Epaper_Write_Data(0x02);  
    
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD::EPD_WhiteScreen_ALL(const unsigned char * BWdatas,const unsigned char * Rdatas)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&BWdatas[i]));
   }
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(~(pgm_read_byte(&Rdatas[i])));
   }


   EPD_Update();   
}

//////////////////clean/////////////
void EPD::EPD_WhiteScreen_ALL_Clean(void)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0xff);
   }
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0x00);
   }


   EPD_Update();   
}

///////////////刷新方式//////
void EPD::EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control 
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}

/////////睡眠模式SSD1680/////////
void EPD::EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}

void EPD::EPD_SSD1680_DISPLAY_TEXT(void)
{
   EPD_HW1_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(gImage_BW,gImage_R); //Refresh the picture in full screen
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!! 
    delay(3000);   
    
    //Clean
    EPD_HW1_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL_Clean();
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); 
    while(1);
}








