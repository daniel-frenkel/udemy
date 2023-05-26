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
  move_to_step = 100000; //Change this value to set the position to move to (Negative will reverse)
  set_velocity = 20000;
  set_accel = 5000;
  set_current = 600;
  // IF StallGuard does not work, it's because these two values are not set correctly or your pins are not correct.
  set_stall = 80;      //Do not set the value too high or the TMC will not detect it. Start low and work your way up.
  set_tcools = 200;   // Set slightly higher than the max TSTEP value you see

  //Do not change these
  driver.rms_current(set_current);
  driver.SGTHRS(set_stall);
  driver.TCOOLTHRS(set_tcools);
  stepper->setSpeedInHz(set_velocity);
  stepper->setAcceleration(set_accel);
  stepper->setCurrentPosition(0);
  //digitalWrite(ENABLE_PIN, LOW);
  run_motor = true;
}

void loop()
{
  stalled_motor = false;
  stepper->moveTo(move_to_step);
    while (stepper->getCurrentPosition() != stepper->targetPos())
    {

      Serial.print("SG_RESULT: ");
      Serial.println(driver.SG_RESULT());
      Serial.print("TSTEP: ");
      Serial.println(driver.TSTEP()); //Check TSTEP value
      
      if (stalled_motor == true)
      {
        Serial.println("Stalled");
        stepper->forceStop();
        break;
      }
    }
    
  stalled_motor = false;
  stepper->moveTo(0);

      while (stepper->getCurrentPosition() != stepper->targetPos())
    {
      Serial.print("SG_RESULT: ");
      Serial.println(driver.SG_RESULT());
      Serial.print("TSTEP: ");
      Serial.println(driver.TSTEP());
   
      if (stalled_motor == true)
      {
        Serial.println("Stalled");
        stepper->forceStop();
        break;
      }
    }
}
