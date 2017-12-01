#include <Arduino.h>
#include <U8g2lib.h>
#include "voc_40.h"
#include "t_40.h"
#include "rh_40.h"
#include "p_40.h"
#include "sd_10.h"
#include <SPI.h>
#include <SdFat.h>     // SD-Library
#include <CircularBuffer.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define U8G2_16BIT
#define SCK 3
#define MISO 2
#define MOSI 4
#define CS_BME 1
#define CS_EINK 0


Adafruit_BME280 bme(CS_BME, MOSI, MISO, SCK); // software SPI
//U8G2_SSD1607_200X200_F_3W_SW_SPI u8g2(U8G2_R3, /* clock=*/ SCK, /* data=*/ MOSI, /* cs=*/ CS_EINK);  // eInk/ePaper Display, original LUT from embedded artists
U8G2_IL3820_V2_296X128_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* cs=*/ 0, /* dc=*/ 1);  // ePaper Display, lesser flickering and faster speed, enable 16 bit mode for this display!

CircularBuffer<int, 60> vocbuffer;
CircularBuffer<float, 60> tbuffer;
CircularBuffer<float, 60> hbuffer;
CircularBuffer<float, 60> pbuffer;
// End of constructor list
int graph_height = 38; //diagnostic initial values
int graph_width =64;
int maxvoc =0;
int minvoc =1000;
int maxt =0;
int mint =100;
int maxh =0;
int minh =100;
int maxp =0;
int minp =100;
int timetextshift=0;

SdFat sd;
void setup() {

 Serial.begin(115200);
 Serial.println("Program started");
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setDrawColor(0);
  u8g2.setPowerSave(1);
 // bme.begin();
//sd.begin();
  vocbuffer.push(0); // Set beginning of graph to 0,0
  tbuffer.push(0); // Set beginning of graph to 0,0
  hbuffer.push(0); // Set beginning of graph to 0,0
  pbuffer.push(0); // Set beginning of graph to 0,0
}

void checkminmax(){ //reset maximum values and determine max storaged value
maxvoc=0; 
minvoc=1000;  
  maxt=0;
  mint=100; 
  maxh=0;
  minh=100;
  maxp=0;
  minp=100;
for (int i=0; i < vocbuffer.size(); i++){
 if (vocbuffer[i] > maxvoc){maxvoc = vocbuffer[i];}
// if (vocbuffer[i] < minvoc){minvoc = vocbuffer[i];}
}
for (int i=0; i < tbuffer.size(); i++){
 if (tbuffer[i] > maxt){maxt = tbuffer[i];}
// if (tbuffer[i] < mint){mint = tbuffer[i];}
}
for (int i=0; i < hbuffer.size(); i++){
 if (hbuffer[i] > maxh){maxh = hbuffer[i];}
// if (hbuffer[i] < minh){minh = hbuffer[i];}
}
for (int i=0; i < pbuffer.size(); i++){
 if (pbuffer[i] > maxp){maxp = pbuffer[i];}
// if (pbuffer[i] < minp){minp = pbuffer[i];}
}
}

void loop() {
for (int i=0; i < 5; i++){
  vocbuffer.push(random(1,30));
  tbuffer.push(random(1,30));
  hbuffer.push(random(1,30));
  pbuffer.push(random(1,30));
}
  checkminmax();

u8g2.setPowerSave(0);

 u8g2.clearBuffer();         // clear the internal memory
  u8g2.setDrawColor(1);
 u8g2.drawBox(0,0,296,128);
  u8g2.setDrawColor(0);
  drawgraph(75,47,'v');
  drawgraph(75,87,'t');
  drawgraph(223,47,'h');
  drawgraph(223,87,'p');
  drawtime();
  drawstatus();
  u8g2.sendBuffer();         // transfer internal memory to the display
u8g2.setPowerSave(1); 
  delay(2000);

}
void drawtime(){
 char* timestring ="18:30";
  u8g2.setFont(u8g2_font_inb24_mf);
 int timex= u8g2.getStrWidth(timestring);

 u8g2.drawStr(148-0.5*timex , 25, timestring);
  u8g2.setFont(u8g2_font_6x10_tr);
}

void drawstatus(){
  int x = 296-21;
  int y = 0;
  int batstate =3;
    u8g2.drawFrame(x,y,19,10);
    u8g2.drawLine(295,3,295,6);
    u8g2.drawLine(294,2,294,7);
  for (int i=0; i < batstate; i++){
    u8g2.drawBox(x+2+(i*4),y+2,3,6);
  }
  u8g2.drawXBM( 296-21-12, 0, sd_10_width, sd_10_height, sd_10_bits); //draw picture
}

void drawgraph(int x, int y, char parameter) {
  u8g2.drawLine(x, y, x, y+graph_height); //y-axis
  u8g2.drawTriangle(x,y, x-3,y+3, x+4,y+3); //arrowhead y-axis
  u8g2.drawLine(x,y+graph_height,x+graph_width, y+graph_height); //x-axis
  u8g2.drawTriangle(x+graph_width,y+graph_height, x+graph_width-2,y+graph_height-3, x+graph_width-2,y+graph_height+3);// arrowhead x-axis
  //u8g2.drawStr(x-22,y+graph_height-1+2, "123");   
  //u8g2.drawStr(x-22,y+6+5, "456");   //-5: half text height
 
  u8g2.drawStr( x+timetextshift-9, 200, "now");
 int mini=0;
 int maxi=0;
 double vocscaler = 1;//(1/40.00)*(maxvoc-minvoc);
 double tscaler = 1;//(1/40.00)*(maxt-mint);
 double hscaler = 1;//(1/40.00)*(maxh-minh);
 double pscaler = 1;//(1/40.00)*(maxh-minh);
  switch (parameter){
    case 'v':
      u8g2.drawXBM( x-75, y, voc_40_width, voc_40_height, voc_40_bits); //draw picture
             
            
      for (int i=1; i < vocbuffer.size(); i++){
 
        u8g2.drawLine(x+i,y+graph_height-vocbuffer[i-1]/vocscaler,x+1+i,y+graph_height-vocbuffer[i]/vocscaler); // draw graph itself
        timetextshift=i;} // only has to be done for one of the graphs
        
      maxi=maxvoc;
      mini=minvoc;

      break;
    case 't':
      u8g2.drawXBM( x-75, y, t_40_width, t_40_height, t_40_bits); //draw picture
      for (int i=0; i < tbuffer.size(); i++){
        u8g2.drawLine(x+i,y+graph_height-tbuffer[i-1]/tscaler,x+1+i,y+graph_height-tbuffer[i]);
        } // draw graph itself
      maxi=maxt;
      mini=mint;
              Serial.println(tbuffer.size());
      
      break;
    case 'h':
      u8g2.drawXBM( x-75, y, rh_40_width, rh_40_height, rh_40_bits); //draw picture
      for (int i=1; i < hbuffer.size(); i++){
        u8g2.drawLine(x+i,y+graph_height-hbuffer[i-1]/hscaler,x+1+i,y+graph_height-hbuffer[i]/hscaler);} // draw graph itself
      maxi=maxh;
      mini=minh;

      break;
    case 'p':
      u8g2.drawXBM( x-75, y, p_40_width, p_40_height, p_40_bits); //draw picture
      for (int i=1; i < hbuffer.size(); i++){
        u8g2.drawLine(x+i,y+graph_height-pbuffer[i-1]/pscaler,x+1+i,y+graph_height-pbuffer[i]/pscaler);} // draw graph itself
      maxi=maxh;
      mini=minh;

      break;

  }
    u8g2.drawLine(x,y+graph_height-1,x-3,y+graph_height-1); //draw line for min value
  u8g2.drawLine(x,y+6,x-3,y+6); //draw line for max value
  
  u8g2.setCursor(x-22,y+graph_height-1+2); //cursor for min value
  u8g2.print(mini);
  u8g2.setCursor(x-22,y+6+5); //cursor for min value
  u8g2.print(maxi);

}

