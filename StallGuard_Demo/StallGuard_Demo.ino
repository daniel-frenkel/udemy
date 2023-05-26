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
  set_velocity = 5000;
  set_current = 600;  
  set_stall = 20;     //Do not set the value too high or the TMC will not detect it. Start low and work your way up
  set_tcools = 1000;  //5000 Velocity = 800 TSTEP

  //Do not change these
  driver.VACTUAL(set_velocity); //We use the internal pulse generator of the TMC2209 so that we can monitor the SG_RESULT
  driver.rms_current(set_current);
  driver.SGTHRS(set_stall);
  driver.TCOOLTHRS(set_tcools);
  digitalWrite(ENABLE_PIN, LOW);
}

void loop()
{
  Serial.println(driver.SG_RESULT());
  if (stalled_motor == true)
  {
    Serial.println("STALLED");
    stalled_motor = false;
  }
  delay(50); //Change delay for faster SG_RESULT values
}
