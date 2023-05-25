#include <Arduino.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include "MotorControl.h"

void setup() {
  Serial.begin(115200);
  setup_motors();

  //We use the internal pulse generator of the TMC2209 so that we can monitor the SG_RESULT
  driver.VACTUAL(5000);
}

void loop()
{
  Serial.println(driver.SG_RESULT());
  if(stalled_motor)
  {
  Serial.println("STALLED");
  stalled_motor = false;
  }
}
