#include <Arduino.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>

/* STEP PIN SETUP: THESE PINS ONLY
   AVR ARmega328p: only Pin 9 and 10.
   AVR ARmega32u4: only Pin 9, 10 and 11.
   AVR ARmega2560: only Pin 6, 7 and 8.
   ESP32: This can be any output capable port pin.
   Atmel Due: This can be one of each group of pins: 34/67/74/35, 17/36/72/37/42, 40/64/69/41, 9, 8/44, 7/45, 6
*/
#define STEP_PIN         6
#define DIR_PIN          3
#define ENABLE_PIN       51
#define RXD2             16
#define TXD2             17
#define STALLGUARD       21
#define R_SENSE          0.11f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2

//Change these values to get different results
long long  move_to_step = 100000; //Change this value to set the position to move to (Negative will reverse)
long  set_velocity = 20000;
int  set_accel = 5000;
int  set_current = 600;

// IF StallGuard does not work, it's because these two values are not set correctly or your pins are not correct.
int  set_stall = 80;      //Do not set the value too high or the TMC will not detect it. Start low and work your way up.
long  set_tcools = 200;   // Set slightly higher than the max TSTEP value you see

bool stalled_motor = false;
int motor_microsteps = 64;
long long current_position = 0;

// We communicate with the TMC2209 over UART
// But the Arduino UNO only have one Serial port which is connected to the Serial Monitor
// We can use software serial on the UNO, and hardware serial on the ESP32 or Mega 2560
#if defined(ESP32) || defined(__AVR_ATmega2560__)
#define SERIAL_PORT_2    Serial2    // TMC2209/TMC2226 HardwareSerial port
#else
#include<SoftwareSerial.h>
SoftwareSerial SERIAL_PORT_2 (2, 3);   //DPin-2 will work as SRX-pin, and DPin-3 will work as STX-pin; S for soft
#endif

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;
TMC2209Stepper driver(&SERIAL_PORT_2, R_SENSE , DRIVER_ADDRESS);

// Interrupt
// This interrupt will fire when a HIGH rising signal is detected on the DIAG pin. This indicates a stall.
#if defined(ESP32) //ESP32 needs special "IRAM_ATTR" in interrupt
void IRAM_ATTR stalled_position()
{
  stalled_motor = true;
}
#else
void stalled_position() //ATmega does not need need special "IRAM_ATTR" in interrupt
{
  stalled_motor = true;
}
#endif

void setup() {
  Serial.begin(115200);
  #if defined(ESP32)
  SERIAL_PORT_2.begin(115200, SERIAL_8N1, RXD2, TXD2); // ESP32 can use any pins to Serial
  #else
  SERIAL_PORT_2.begin(115200);
  #endif
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STALLGUARD , INPUT);
  attachInterrupt(digitalPinToInterrupt(STALLGUARD), stalled_position, RISING);
  
  driver.begin(); // Start all the UART communications functions behind the scenes
  driver.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  driver.blank_time(24); //Recommended blank time select value
  driver.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  driver.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  driver.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  driver.microsteps(motor_microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  driver.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  driver.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  driver.shaft(true); // Set the shaft direction. Only use this command one time during setup to change the direction of your motor. Do not use this in your code to change directions.
  driver.en_spreadCycle(false); // Disable SpreadCycle. We want StealthChop becuase it works with StallGuard.
  driver.pdn_disable(true); // Enable UART control
  driver.VACTUAL(0); // Enable UART control
  driver.rms_current(set_current);
  driver.SGTHRS(set_stall);
  driver.TCOOLTHRS(set_tcools);
  
  engine.init();
  stepper = engine.stepperConnectToPin(STEP_PIN);
  stepper->setDirectionPin(DIR_PIN);
  stepper->setEnablePin(ENABLE_PIN);
  stepper->setAutoEnable(true);
  stepper->setSpeedInHz(set_velocity);
  stepper->setAcceleration(set_accel);
  stepper->setCurrentPosition(0);

}

void loop()
{
  stalled_motor = false; // We'll set the stall flag to false, in case it was triggered easlier.
  stepper->moveTo(move_to_step); // We tell the motor to move to a spcific position.
  while (stepper->getCurrentPosition() != stepper->targetPos()) // While the current position DOES NOT equal the target position, we can do some things. You can just add a delay here as well to kill time.
  {

    Serial.print("SG_RESULT: "); 
    Serial.println(driver.SG_RESULT()); // Print the SG_RESULT value when you are tuning stallGuard
    Serial.print("TSTEP: ");
    Serial.println(driver.TSTEP()); //Print the TSTEP value when you are tuning stallGuard

    if (stalled_motor == true) // If the motor stall, this will print "Stalled"
    {
      Serial.println("Stalled");
      stepper->forceStop(); // This instantly stops the motor.
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
