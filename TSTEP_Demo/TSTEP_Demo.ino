//Set your correct pins on MotorControl.h!

#include <Arduino.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include "MotorControl.h"

void setup() {
  Serial.begin(115200);
  setup_motors();

  //Change these values to get different results
  set_velocity = 30000;  
  set_current = 600;     
  set_stall = 20;        //Do not set the value too high or the TMC will not detect it. Start low and work your way up
  set_tcools = 200;      //Watch the Serial monitor. Try to stall the motor with your fingers. Notice that is won't stall with LARGER than this value

  //Do not change these
  driver.VACTUAL(0); //We use the internal pulse generator of the TMC2209 so that we can monitor the SG_RESULT
  driver.rms_current(set_current);
  driver.SGTHRS(set_stall);
  driver.TCOOLTHRS(set_tcools);
  digitalWrite(ENABLE_PIN, LOW);
}

void loop()
{
  Serial.println(driver.TSTEP());
  for (int i = 0; i < set_velocity; i = i + 10)
  {
    driver.VACTUAL(i);
    Serial.println(driver.TSTEP());
    if (stalled_motor)
    {
      Serial.println("STALLED");
      stalled_motor = false;
    }
  }

  for (int i = set_velocity; i > 0; i = i - 10)
  {
    driver.VACTUAL(-i);
    Serial.println(driver.TSTEP());
    if (stalled_motor)
    {
      Serial.println("STALLED");
      stalled_motor = false;
    }
  }
}