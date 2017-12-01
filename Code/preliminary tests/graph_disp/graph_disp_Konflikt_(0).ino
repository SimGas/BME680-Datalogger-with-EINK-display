/*

  HelloWorld.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <CircularBuffer.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

CircularBuffer<float, 100> buffer;
// End of constructor list
int graph_height = 20;
int graph_width =0;

void setup(void) {
  u8g2.begin();
graph_height = u8g2.getDisplayHeight()/2;
graph_width = 100;//u8g2.getDisplayWidth()*5/6;
for (int i=0; i <= 255; i++){
    
}

     //u8g2.setDisplayRotation(U8G2_R2);
}

void loop(void) {
  u8g2.clearBuffer();					// clear the internal memory
  buffer.push(random(1,30));
  draw();
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(100);  
}


void draw() {
  //u8g2.getDisplayWidth(),u8g2.getDisplayHeight()
u8g2.setFont(u8g2_font_6x10_mr);  // choose a suitable font
 // u8g2.drawStr( 0, 10, "VOC");
  u8g2.drawLine(10, 10, 10, 10+graph_height); //y-axis
  u8g2.drawTriangle(10,10, 10-3,10+3, 10+4,10+3); //arrowhead y-axis
  u8g2.drawLine(10, 10+graph_height, 10+ graph_width, 10+graph_height); //x-axis
  u8g2.drawTriangle(10+graph_width,10+graph_height, 10+graph_width-2,10+graph_height-3, 10+graph_width-2,10+graph_height+3);// arrowhead x-axis
  u8g2.drawStr( graph_width, 20+graph_height, "t");
  for (int i=1; i <= buffer.size(); i++){
  u8g2.drawLine(10+i,10+graph_height-buffer[i-1],10+1+i,10+graph_height-buffer[i]);  
  }
}



