/*
DS3231_test.pde
Eric Ayars
4/11

Test/demo of read routines for a DS3231 RTC.

Turn on the serial monitor after loading this to check if things are
working as they should.

*/

#include <DS3231.h>
#include <Wire.h>
#define RTC_INT 15

DS3231 Clock;
bool Century=false;
bool h12;
bool PM_time;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
int interruptcounter=0;
void setup() {
	// Start the I2C interface
	Wire.begin();
	// Start the serial interface
	Serial.begin(9600);
// Test of alarm functions
    // set A1 to one minute past the time we just set the clock
    // on current day of week.
    //Clock.setA1Time(0, 22, 13, 10, 0b0001110, true,false, false); // Alarm 1 uses last four bit
    // set A2 to two minutes past, on current day of month.   // Alarm 2 uses first three bit
    //Clock.setA2Time(29, 22, 50, 0b1110000, false, false,false);
    // Turn on both alarms, with external interrupt
    //Clock.turnOffAlarm(1);
   // Clock.turnOnAlarm(2);
   pinMode(RTC_INT, INPUT_PULLUP);
   attachInterrupt(RTC_INT,inthandler, FALLING);
}

void inthandler(){
interruptcounter++;
  
}

void loop() {
	// send what's going on to the serial monitor.
	// Start with the year
  Serial.print("Interrupts so far:");
  Serial.println(interruptcounter);
	Serial.print("2");
	if (Century) {			// Won't need this for 89 years.
		Serial.print("1");
	} else {
		Serial.print("0");
	}
	Serial.print(Clock.getYear(), DEC);
	Serial.print(' ');
	// then the month
	Serial.print(Clock.getMonth(Century), DEC);
	Serial.print(' ');
	// then the date
	Serial.print(Clock.getDate(), DEC);
	Serial.print(' ');
	// and the day of the week
	Serial.print(Clock.getDoW(), DEC);
	Serial.print(' ');
	// Finally the hour, minute, and second
	Serial.print(Clock.getHour(h12, PM_time), DEC);
	Serial.print(' ');
	Serial.print(Clock.getMinute(), DEC);
	Serial.print(' ');
	Serial.print(Clock.getSecond(), DEC);
	// Add AM/PM indicator
	if (h12) {
		if (PM_time) {
			Serial.print(" PM ");
		} else {
			Serial.print(" AM ");
		}
	} else {
		Serial.print(" 24h ");
	}
	// Display the temperature
	Serial.print("T=");
	Serial.print(Clock.getTemperature(), 2);
	// Tell whether the time is (likely to be) valid
	if (Clock.oscillatorCheck()) {
		Serial.print(" O+");
	} else {
		Serial.print(" O-");
	}
	// Indicate whether an alarm went off
	if (Clock.checkIfAlarm(1)) {
		Serial.print(" A1!");
   int second = Clock.getSecond();
   int alarmsecond =0;
   if (second >= 50){alarmsecond=50-second;}
   else{alarmsecond = second+10;}
    Clock.setA1Time(0, 22, 13, alarmsecond, 0b1110, false,false, false);
	}
	if (Clock.checkIfAlarm(2)) {
		Serial.print(" A2!");
	}
	// New line on display
	Serial.print('\n');
	// Display Alarm 1 information
	Serial.print("Alarm 1: ");
	Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
	Serial.print(ADay, DEC);
	if (ADy) {
		Serial.print(" DoW");
	} else {
		Serial.print(" Date");
	}
	Serial.print(' ');
	Serial.print(AHour, DEC);
	Serial.print(' ');
	Serial.print(AMinute, DEC);
	Serial.print(' ');
	Serial.print(ASecond, DEC);
	Serial.print(' ');
	if (A12h) {
		if (Apm) {
			Serial.print('pm ');
		} else {
			Serial.print('am ');
		}
	}
	if (Clock.checkAlarmEnabled(1)) {
		Serial.print("enabled");
    
	}
	Serial.print('\n');
	// Display Alarm 2 information
	Serial.print("Alarm 2: ");
	Clock.getA2Time(ADay, AHour, AMinute, ABits, ADy, A12h, Apm);
	Serial.print(ADay, DEC);
	if (ADy) {
		Serial.print(" DoW");
	} else {
		Serial.print(" Date");
	}
	Serial.print(' ');
	Serial.print(AHour, DEC);
	Serial.print(' ');
	Serial.print(AMinute, DEC);
	Serial.print(' ');
	if (A12h) {
		if (Apm) {
			Serial.print('pm');
		} else {
			Serial.print('am');
		}
	}
	if (Clock.checkAlarmEnabled(2)) {
		Serial.print("enabled");
	}
	// display alarm bits
	Serial.print("\nAlarm bits: ");
	Serial.print(ABits, BIN);

	Serial.print('\n');
	Serial.print('\n');
	delay(1000);
}
