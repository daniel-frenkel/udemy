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
  max_speed = 5000;
  current = 1000;
  stall = 10;
  tcools = 0;

}

void loop()
{
  //Serial.println(driver.TSTEP());

  for (int i = 0; i < max_speed; i = i + 100)
  {
    max_speed = i;
    driver.VACTUAL(max_speed);
    Serial.println(driver.TSTEP());
    
    if (stalled_motor)
    {
      Serial.println("STALLED");
      stalled_motor = false;
    }
  }

  for (int i = max_speed; i > 0; i = i - 100)
  {
    max_speed = i;
    driver.VACTUAL(max_speed);
    Serial.println(driver.TSTEP());

    if (stalled_motor)
    {
      Serial.println("STALLED");
      stalled_motor = false;
    }
  }
}
