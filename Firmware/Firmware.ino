#include <Arduino.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include "MotorControl.h"

void setup() {
  Serial.begin(115200);
  setup_motors();

  //Change this value to set the position (Negative will reverse)
  move_to_step = 5000;
  run_motor = true;
}

void loop()
{
  if (run_motor == true)
  {
    Serial.println("Run Motor Function");
    move_motor();
    run_motor = false;
    Serial.println("Motor Complete");
  }
}
