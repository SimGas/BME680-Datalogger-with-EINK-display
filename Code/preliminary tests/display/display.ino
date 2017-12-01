#include <Arduino.h>
#include <U8g2lib.h>
#include "bme680_200_mono.h" 
#include <SPI.h>
#include <SdFat.h>     // SD-Library

U8G2_SSD1607_200X200_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 0, /* dc=*/ 1, /* reset=*/ 2);  // eInk/ePaper Display, original LUT from embedded artists


void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
   u8g2.setDisplayRotation(U8G2_R2);


}

void loop() {

draw();
delay(2000);
}

void draw()
{
  u8g2.firstPage();
  do {
    u8g2.drawStr(0,15,"Hello World!");    
    u8g2.drawXBM( 0, 142, bme680_200_mono_width, bme680_200_mono_height, bme680_200_mono_bits); 
  //  u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight() );
  } while ( u8g2.nextPage() );
  delay(2000);
}

