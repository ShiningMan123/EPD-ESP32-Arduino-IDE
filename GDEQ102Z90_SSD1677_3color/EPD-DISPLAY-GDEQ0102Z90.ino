
#include "EPD.h"
EPD EPD;




void setup() {
  // put your setup code here, to run once:
EPD.GPIO_set();
}

void loop() {
  // put your main code here, to run repeatedly:
EPD.EPD_SSD1680_DISPLAY_TEXT();
}
