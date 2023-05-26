#define STEP_PIN         52
#define DIR_PIN          53
#define ENABLE_PIN       51
#define RXD2             16
#define TXD2             17
#define STALLGUARD       21
#define R_SENSE          0.11f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2

bool stalled_motor = false;
int move_to_step;
int set_current;
int set_stall;
int set_accel;
int set_velocity;
int set_tcools;
int motor_microsteps = 64;
bool run_motor = false;
int current_position = 0;

// We communicate with the TMC2209 over UART
// But the Arduino UNO only have one Serial port which is connected to the Serial Monitor
// We can use software serial on the UNO, and hardware serial on the ESP32 or Mega 2560
#if defined(ESP32) || defined(__AVR_ATmega2560__)
#define SERIAL_PORT_2    Serial2    // TMC2208/TMC2224 HardwareSerial port
#else
#include<SoftwareSerial.h>
SoftwareSerial SERIAL_PORT_2 (2, 3);   //DPin-2 will work as SRX-pin, and DPin-3 will work as STX-pin; S for soft
#endif

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;
TMC2209Stepper driver(&SERIAL_PORT_2, R_SENSE , DRIVER_ADDRESS);

#if defined(ESP32) //ESP32 needs special "IRAM_ATTR" in interrupt
void IRAM_ATTR stalled_position()
{
  stalled_motor = true;
}
#else
void stalled_position()
{
  stalled_motor = true;
}
#endif


void move_motor() {
  stalled_motor = false;
  if (current_position == move_to_step)
  {
    Serial.println("ALREADY THERE!");
  }
  else
  {
    Serial.println("Moving");
    stepper->moveTo(move_to_step);
    while (stepper->getCurrentPosition() != stepper->targetPos())
    {
      if (stalled_motor == true)
      {
        printf("Stalled\n");
        stepper->forceStop();
        break;
      }
    }
  }
  current_position = stepper->getCurrentPosition();
  printf("Motor Function Complete\n");
}

void setup_motors(){
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STALLGUARD , INPUT);
  attachInterrupt(digitalPinToInterrupt(STALLGUARD), stalled_position, RISING);
  SERIAL_PORT_2.begin(115200);
  driver.begin(); // Start all the UART communications functions behind the scenes
  driver.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  driver.blank_time(24); //Recommended blank time select value
  driver.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  driver.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  driver.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  driver.microsteps(motor_microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  driver.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  driver.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  driver.shaft(true); // Set the shaft direction.
  driver.en_spreadCycle(false); // Disable SpreadCycle. We want StealthChop becuase it works with StallGuard.
  driver.pdn_disable(true); // Enable UART control

  engine.init();
  stepper = engine.stepperConnectToPin(STEP_PIN);
  stepper->setDirectionPin(DIR_PIN);
  stepper->setEnablePin(ENABLE_PIN);
  stepper->setAutoEnable(true);
}
