#include <Arduino.h>
#include <U8g2lib.h>
#include "voc_40.h"
#include "t_40.h"
#include "rh_40.h"
#include "p_40.h"
#include "sd_10.h"
#include <SPI.h>
#include <Wire.h>
#include <SdFat.h>     // SD-Library
#include <CircularBuffer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"
#define SD_SCK_MHZ(maxMhz) SPISettings(1000000UL*maxMhz, MSBFIRST, SPI_MODE0)
#define SPI_DIV3_SPEED SD_SCK_HZ(F_CPU/3)
//#define U8G2_16BIT // has to be done in U8G2-sourcefile
#define RST_EINK 12
#define DC_EINK 6
#define CS_EINK 10 
#define CS_SD 11
#define VBATPIN 9
#define RTC_INT 15 // For Arduino Zero, Pin number is required, not interrupt number 
#define SD_CardDetect 19

bool color = 1; //color switch. 1= black on white, 0 = white on black


const char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
const char months[12][11] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August","September", "Oktober", "November", "Dezember"};

Adafruit_BME280 bme;
//DS3231 rtc_alarm;
RTC_DS3231 rtc;
U8G2_IL3820_V2_296X128_F_4W_HW_SPI u8g2(U8G2_R0, CS_EINK, DC_EINK, RST_EINK );
SdFat SD;
CircularBuffer<int, 60> vocbuffer;
CircularBuffer<float, 60> tbuffer;
CircularBuffer<float, 60> hbuffer;
CircularBuffer<float, 60> pbuffer;
// End of constructor list
int graph_height = 38; //diagnostic initial values
int graph_width =64;
float maxvoc =0;
float minvoc =1000;
float maxt =0;
float mint =100;
float maxh =0;
float minh =100;
float maxp =0;
float minp =100;
bool warning= false;
bool interruptflag = false;
volatile bool interuptFlag = false;
int timetextshift=0;
String timestring = "123";
File datafile;
uint32_t logTime;
char timestamp[19] = "20171201;22:30:00";
char filename[13] = "20171201.csv";
   DateTime now ; //has to be a global variable


void setup() {

 Serial.begin(115200);
 Serial.println("Program started");
  pinMode(SD_CardDetect, INPUT);

 
 Wire.begin(); //needed for DS3231 library
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setDrawColor(color);
  u8g2.setPowerSave(1);
  bme.begin();
  vocbuffer.push(0); // Set beginning of graph to 0,0
  tbuffer.push(bme.readTemperature());
  hbuffer.push(bme.readHumidity());
  pbuffer.push(bme.readPressure() / 100.0F);
  
  
  //rtc.setAlarm1Simple(now.hour(), now.minute(), now.second() + 4);
  rtc.setA1Time(0, 0, 0, 10,0b00001110, false, false, false); /// 1110 means: Alarm, when seconds(10) match
  rtc.setA2Time(0, 0, 0, 0b1110000, false, false, false); //111 mens: Alarm every minute, at second 00
  rtc.turnOnAlarm(1);
  rtc.turnOnAlarm(2);
  
if (!SD.begin(CS_SD, SPI_DIV3_SPEED)) {
    Serial.println("Card failed, or not present");
    warning=1;
    // don't do anything more:
    return;                 //Don't place any SD-unrelated code below here:##############################################################################
  }
  Serial.println("card initialized.");
  createfilename();
  writeheader();
}

void ISR(){
interruptflag=1; 
}

//#########################################################################################################################################################
//#################################################################### Loops ##############################################################################
//#########################################################################################################################################################

/*void loop() {
  now = rtc.now(); //Both loops need clock. now is initialized as global Datetime
  bool runmeasureloop = rtc.checkIfAlarm(1);
  bool rundisplayloop = rtc.checkIfAlarm(2);
    
    Serial.print(timestamp);
    Serial.println(" still running");
    if (runmeasureloop) {
          Serial.print(timestamp);
    Serial.print("  ");
    Serial.println("Sensorloop initiated");
   int second = now.second();
   int alarmsecond =0;
   if (second >= 50){alarmsecond=second-50;}
   else{alarmsecond = second+10;}
    rtc.setA1Time(0, 22, 13, alarmsecond, 0b1110, false,false, false);
    measureloop();
  }
  if (rundisplayloop) {
    Serial.print(timestamp);
    Serial.print("  ");
    Serial.println("Displayloop initiated");
     displayloop();
  }
    
   
delay(500);
}*/


void loop(){
now =rtc.now();
measureloop();
displayloop();
delay(2000);
Serial.println(now.second());
}


//#################################################################### Measure loop ######################################################################

void measureloop(){
sprintf(timestamp, "%d%02d%02d;%02d:%02d:%02d", now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second());
vocbuffer.push(random(1,30));
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = (bme.readPressure()/ 100.0F);
  tbuffer.push(temperature);
  hbuffer.push(humidity);
  pbuffer.push(pressure);
  checkminmax();
  double measuredvbat = analogRead(VBATPIN);
measuredvbat *= 2;    // we divided by 2, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage
  String datastring = timestamp;
    datastring+= ";";
    datastring += String(measuredvbat,3);
    datastring+= ";";
    datastring+= temperature;
    datastring+= ";";
    datastring+= humidity;
    datastring+= ";";
    datastring+= pressure;
//if(warning==1 & digitalRead(SD_CardDetect)){SD.begin();Serial.println("looking for SD-card");}
if (datafile = SD.open(filename, FILE_WRITE)) {  
  datafile.println(datastring); //finish and write
  datafile.close();
  warning=0;
  }
  else {
    Serial.println("error opening datafile");
    Serial.print("SD-Error: ");
//    Serial.println(SD.readError());
    warning=1;
  }
  if (!datafile.sync() || datafile.getWriteError()) {
    Serial.println("write error");
    warning=1;
  }
}
//#################################################################### Display loop ######################################################################
void displayloop(){
u8g2.setPowerSave(0);
 u8g2.clearBuffer();         // clear the internal memory
 u8g2.setDrawColor(not(color));
 u8g2.drawBox(0,0,296,128);
 u8g2.setDrawColor(color);
 drawgraph(75,47,'v');
 drawgraph(75,87,'t');
 drawgraph(223,47,'h');
 drawgraph(223,87,'p');
 drawvalue(0,10,'t');
 drawvalue(0,20,'v');
 drawvalue(0,30,'p');
 drawvalue(0,40,'h');
 drawtime();
 drawdate();
 drawstatus();
 u8g2.sendBuffer();         // transfer internal memory to the display
u8g2.setPowerSave(1); 
}
//#########################################################################################################################################################
//#################################################################### Misc Functions #####################################################################
//#########################################################################################################################################################

void dateTime(uint16_t* date, uint16_t* time) {
  now = rtc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}


void createfilename(){
  DateTime now = rtc.now();/*
filename = String(now.year(),DEC);
filename += String(now.month()/10,DEC); //just for getting two characters per month: 2/3 -> 02/03
filename += String(now.month()%10,DEC);
filename += String(now.day()/10,DEC);
filename += String(now.day()%10,DEC);
filename += String(now.hour()/10,DEC);
filename += String(now.hour()%10,DEC);
filename += String(now.minute()/10,DEC);
filename += String(now.minute()%10,DEC);
filename += String(now.second()/10,DEC);
filename += String(now.second()%10,DEC);
*/
sprintf(filename, "%d%02d%02d.csv", now.year(),now.month(),now.day());
}

void writeheader() {
DateTime now = rtc.now();
String datastring = "File creation date:\r\n";
sprintf(timestamp, "%d%02d%02d;%02d:%02d:%02d", now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second());
datastring += timestamp;
datastring += "\r\n";
datastring += "Date;Time;U_Battery;Temperature;Humidity;Pressure";
SdFile::dateTimeCallback(dateTime);
datafile = SD.open(filename, FILE_WRITE);
datafile.println(datastring);
datafile.close();

}

void checkminmax(){ //reset maximum values and determine max storaged value
maxvoc=0; 
minvoc=1000;  
  maxt=0;
  mint=100; 
  maxh=0;
  minh=100;
  maxp=0;
  minp=1100;
for (int i=0; i < vocbuffer.size(); i++){
 if (vocbuffer[i] > maxvoc){maxvoc = vocbuffer[i];}
 if (vocbuffer[i] < minvoc){minvoc = vocbuffer[i];}
}
for (int i=0; i < tbuffer.size(); i++){
 if (tbuffer[i] > maxt){maxt = tbuffer[i];}
 if (tbuffer[i] < mint){mint = tbuffer[i];}
 if (maxt-mint < 1){maxt =maxt+0.5;mint=mint-0.5;}
}
for (int i=0; i < hbuffer.size(); i++){
 if (hbuffer[i] > maxh){maxh = hbuffer[i];}
 if (hbuffer[i] < minh){minh = hbuffer[i];}
 if (maxh-minh < 1){maxh =maxh+0.5;minh=minh-0.5;}
 //if (maxh >100){maxh=100;}

}
for (int i=0; i < pbuffer.size(); i++){
 if (pbuffer[i] > maxp){maxp = pbuffer[i];}
 if (pbuffer[i] < minp){minp = pbuffer[i];}
  if (maxp-minp < 1){maxp =maxp+0.5;minp=minp-0.5;}
}
}
//#########################################################################################################################################################
//#################################################################### Draw routines ######################################################################
//#########################################################################################################################################################
void drawtime(){
char timechar[9] ;
sprintf(timechar, "%02d:%02d",now.hour(),now.minute());
  u8g2.setFont(u8g2_font_inb24_mf);
 int timex= u8g2.getStrWidth(timechar);
 u8g2.drawStr(148-0.5*timex , 25, timechar);
}

void drawstatus(){
if (warning==0){u8g2.drawXBM( 296-21-10, 0, sd_10_width, sd_10_height, sd_10_bits);} //draw SD status
    int x = 296-19;
  int y = 0;
  int batstate =3;
    u8g2.drawFrame(x,y,17,10);
    u8g2.drawLine(295,3,295,6);
    u8g2.drawLine(294,2,294,7);
    
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

  int batpercentage =0;
batpercentage = 205.16*measuredvbat-738,35; // conversion to battery percentage. linear approximation done by writing voltage to sd card and do a linear approximation with excel
if (batpercentage >100){batpercentage=100;}
else if (batpercentage <0){batpercentage =0;}
char batpercentagetext[5];
sprintf(batpercentagetext,"%03d",batpercentage);
u8g2.setFont(u8g2_font_5x7_tr);
int textshift= u8g2.getStrWidth(batpercentagetext);
u8g2.setCursor(296-11-0.5*textshift,y+8); //center text in battery icon
u8g2.print(batpercentagetext);
}

void drawgraph(int x, int y, char parameter) {
  u8g2.drawLine(x, y, x, y+graph_height); //y-axis
  u8g2.drawTriangle(x,y, x-3,y+3, x+4,y+3); //arrowhead y-axis
  u8g2.drawLine(x,y+graph_height,x+graph_width, y+graph_height); //x-axis
  u8g2.drawTriangle(x+graph_width,y+graph_height, x+graph_width-2,y+graph_height-3, x+graph_width-2,y+graph_height+3);// arrowhead x-axis
  //u8g2.drawStr(x-22,y+graph_height-1+2, "123");   
  //u8g2.drawStr(x-22,y+6+5, "456");   //-5: half text height
 
  u8g2.drawStr( x+timetextshift-9, 200, "now");
 float mini=0;
 float maxi=0;
 double vocscaler = 1;//(1/graph_height)*(maxvoc-minvoc);

 float tscaler = float(31/(maxt-mint));//35 is the actual graph height, not the axis height
 float hscaler = float(31/(maxh-minh));
 float pscaler = float(31/(maxp-minp));

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
        u8g2.drawLine(x+i,y+graph_height-1-int((tbuffer[i-1]-mint)*tscaler),x+1+i,y+graph_height-1-int((tbuffer[i]-mint)*tscaler));
        } // draw graph itself
      maxi=maxt;
      mini=mint;


      break;
    case 'h':
      u8g2.drawXBM( x-75, y, rh_40_width, rh_40_height, rh_40_bits); //draw picture
      for (int i=1; i < hbuffer.size(); i++){
        u8g2.drawLine(x+i,y+graph_height-1-int((hbuffer[i-1]-minh)*hscaler),x+1+i,y+graph_height-1-int((hbuffer[i]-minh)*hscaler)); } // draw graph itself
      maxi=maxh;
      mini=minh;

      break;
    case 'p':
      u8g2.drawXBM( x-75, y, p_40_width, p_40_height, p_40_bits); //draw picture
      for (int i=1; i < hbuffer.size(); i++){
        u8g2.drawLine(x+i,y+graph_height-1-int((pbuffer[i-1]-minp)*pscaler),x+1+i,y+graph_height-1-int((pbuffer[i]-minp)*pscaler)); } // draw graph itself
      maxi=maxp;
      mini=minp;

      break;

  }
    u8g2.drawLine(x,y+graph_height-1,x-3,y+graph_height-1); //draw line for min value
  u8g2.drawLine(x,y+6,x-3,y+6); //draw line for max value


  u8g2.setFont(u8g2_font_micro_tr);
  int textshift= u8g2.getStrWidth(String(mini).c_str());
  u8g2.setCursor(x-4-textshift,y+graph_height-1+2); //cursor for min value
  u8g2.print(mini);
  textshift= u8g2.getStrWidth(String(maxi).c_str());
  u8g2.setCursor(x-4-textshift,y+6+3); //cursor for min value
  u8g2.print(maxi);
}

void drawvalue(int x, int y, char parameter){
u8g2.setFont(u8g2_font_6x12_tf);
String currentvalue;
switch (parameter){
    case 'v':
      currentvalue = "IAQ: ";
      currentvalue += String(vocbuffer.last());
    break;
    case 't':
    currentvalue = "T: ";
      currentvalue += String(tbuffer.last(),2);
      currentvalue += "\xB0\C";
      break;
    case 'p':
    currentvalue = "p: ";
      currentvalue += String(pbuffer.last(),0);
      currentvalue += "mbar";
      break;
    case 'h':
      currentvalue = "rh: ";
      currentvalue += String(hbuffer.last(),1);
      currentvalue += "%";
      break;}
  u8g2.setCursor(x,y);
  u8g2.print(currentvalue); 
}

void drawdate(){
String datestring;
now = rtc.now();
datestring = daysOfTheWeek[now.dayOfTheWeek()];
datestring +=", ";
datestring += now.day();
datestring +=". ";
datestring +=months[now.month()-1];
datestring +=" ";
datestring +=now.year();
u8g2.setFont(u8g2_font_6x12_tf);
int textshift= u8g2.getStrWidth(String(datestring).c_str());
u8g2.setCursor(148-textshift/2,35); //cursor for min value
u8g2.print(datestring);
}

