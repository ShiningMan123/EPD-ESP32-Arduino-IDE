#ifndef  EPD_H_
#define  EPD_H_
#include <Arduino.h>


//#define MAX_LINE_BYTES 16// =128/8
//#define MAX_COLUMN_BYTES  250

//#define ALLSCREEN_GRAGHBYTES  4000

class EPD
{
  public:
void GPIO_set(void);
void driver_delay_us(unsigned int xus);
void driver_delay(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
//EPD

void Epaper_READBUSY(void);
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char datas);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_HW1_Init(void);
void EPD_Part_Init(void); //Local refresh initialization
void EPD_HW_Init_Fast(void);

void EPD_Part_Update(void); 
void EPD_Update(void);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_WhiteScreen_ALL(const unsigned char * datas);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_DISPLAY_TEXT(void);
  private:
   String num="";
};//注意这里的分号不要漏掉

#endif
