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
  move_to_step = 10000; //Change this value to set the position to move to (Negative will reverse)
  set_velocity = 5000;
  set_accel = 1000;
  set_current = 600;  
  set_stall = 20;      //Do not set the value too high or the TMC will not detect it. Start low and work your way up
  set_tcools = 1000;   //5000 Velocity = 800 TSTEP

  //Do not change these
  driver.rms_current(set_current);
  driver.SGTHRS(set_stall);
  driver.TCOOLTHRS(set_tcools);
  stepper->setSpeedInHz(set_velocity);
  stepper->setAcceleration(set_accel);
  
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
