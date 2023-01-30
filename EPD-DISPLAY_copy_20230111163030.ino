#include "EPD.h"
EPD EPD;

void setup() {
  // put your setup code here, to run once:

EPD.GPIO_set();
}

void loop() {
//EPD.EPD_UC8253_DISPLAY_TEXT();
EPD.EPD_SSD1680_DISPLAY_TEXT();
}
