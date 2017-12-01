
/*

 Arduino ZERO PRO low-power sleep mode with wakeup upon external interrupt
 (example sketch)

 Add a button on digital pin 0, with an additional pull-up resistor.
 Add an LED on digital pin 3 (don't forget resistor)
 
 NOTE: LED might not appear to toggle, or it might flash, that is because of switch bounce.
 (Electrically noisy contacts)
 I think there is a filtering option to digitally filter external interrupts.
  Might check that out in the future.

*/

bool ledState = true;

void setup()
{
  Serial.begin(9600);
  pinMode(0, OUTPUT);  // Output for an LED that is toggled on/off upon interrupt
  pinMode(13, OUTPUT);  // Flashing LED pin
  pinMode(1, INPUT_PULLUP);
  // I could actually use the ARM macro thingies and set registers
  // and what not to do the same exact thing,
  // But I'm lazy so I just cheated and used this arduino function
  
  attachInterrupt(1, onInt, LOW);
  
  SCB->SCR |= 1<<2; // Enable deep-sleep mode
  
  // Set the EIC (External Interrupt Controller) to wake up the MCU
  // on an external interrupt from digital pin 0. (It's EIC channel 2 on the trinket)
  EIC->WAKEUP.reg = EIC_WAKEUP_WAKEUPEN2;
}

void loop()
{
  Serial.println("Sleeping in 3");
  toggleAndDelay();
  Serial.println("Sleeping in 2");
  toggleAndDelay();
  Serial.println("Sleeping in 1");
  toggleAndDelay();
  __WFI(); // This is the WFI (Wait For Interrupt) function call.
}

// Called upon interrupt of digital pin 0.
void onInt()
{
  ledState = !ledState;
  digitalWrite(0, ledState);
}

// This just toggles the LED on pin 13, and delays.
// Used in between the sleep countdown Serial.println()
void toggleAndDelay()
{
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}


