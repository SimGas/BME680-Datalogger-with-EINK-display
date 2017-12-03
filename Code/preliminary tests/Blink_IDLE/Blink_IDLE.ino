
//#include <sam.h>
#include <power.h>
#include <Adafruit_ASFcore.h>
#define RTC_INT 15
// the setup function runs once when you press reset or power the board
volatile bool interruptflag;
void setup() {
 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RTC_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INT), ISRR, FALLING);
  EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN8;        // Set External Interrupt Controller to use channel 8 - pin 15
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2); // Deepest sleep
}


void ISRR(){
  interruptflag=1;
  }

  
// the loop function runs over and over again forever
void loop() {

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  //interruptflag=0;
while (!(interruptflag))
{
 system_sleep();
}

}


