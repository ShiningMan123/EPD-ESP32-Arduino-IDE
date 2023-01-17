#include "EPD.h"
//#include "GDEY0213B74.h"
//#include ""
EPD EPD;

void setup() {
  // put your setup code here, to run once:

EPD.GPIO_set();
}

void loop() {
EPD.EPD_DISPLAY_TEXT();
}
