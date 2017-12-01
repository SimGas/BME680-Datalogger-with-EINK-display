#include <Arduino.h>
#include <U8g2lib.h>
#include "bme680_200_mono.h" 
#include <SPI.h>
#include <SdFat.h>     // SD-Library
#include <CircularBuffer.h>




U8G2_SSD1607_200X200_F_3W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 0);  // eInk/ePaper Display, original LUT from embedded artists

CircularBuffer<float, 100> buffer;
// End of constructor list
int graph_height = 10; //diagnostic initial values
int graph_width =10;
int buffermax =0;


void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);

  graph_height = u8g2.getDisplayHeight()/3;
  graph_width = 100;//u8g2.getDisplayWidth()*5/6;
  buffer.push(0); // Set beginning of graph to 0,0
}

void loop() {
  u8g2.clearBuffer();          // clear the internal memory
  buffer.push(random(1,30));
  drawgraph();
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);
 int lastbuffer=buffer.last();
if (lastbuffer > buffermax){buffermax = lastbuffer;}
}

void drawgraph()
{

    u8g2.drawStr(0,15,"Hello World!");    
    u8g2.drawXBM( 0, 142, bme680_200_mono_width, bme680_200_mono_height, bme680_200_mono_bits); 
  //  u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight() );
  u8g2.setFont(u8g2_font_6x10_mr);  // choose a suitable font
 // u8g2.drawStr( 0, 10, "VOC");
  u8g2.drawLine(10, 10, 10, 10+graph_height); //y-axis
  u8g2.drawTriangle(10,10, 10-3,10+3, 10+4,10+3); //arrowhead y-axis
  u8g2.drawLine(10, 10+graph_height, 10+ graph_width, 10+graph_height); //x-axis
  u8g2.drawTriangle(10+graph_width,10+graph_height, 10+graph_width-2,10+graph_height-3, 10+graph_width-2,10+graph_height+3);// arrowhead x-axis
  u8g2.drawStr( graph_width, 20+graph_height, "t");
  for (int i=0; i <= buffer.size(); i++){
  u8g2.drawLine(10+i,10+graph_height-buffer[i-1],10+1+i,10+graph_height-buffer[i]);  
  }
}

