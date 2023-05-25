#include <Arduino.h>
#include <ESPUI.h>
#include "FastAccelStepper.h"
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include <Preferences.h>
#include "Memory.h"
#include "ResetButton.h"
#include "MotorControl.h"
#include "API.h"
#include "ESPUI.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  preferences.begin("local", false);
  load_preferences();
  setup_motors();
  API();
  ESPUIsetup();
}

void loop()
{
  if (run_motor == true)
  {
    Serial.println("Run Motor Function");
    move_motor();
    run_motor = false;
    ESPUI.updateLabel(positionLabel, String(int(((float)current_position / (float)max_steps) * 100)));
    Serial.println("Motor Complete");
  }
  else if (set_zero == true)
  {
    setZero();
    set_zero = false;
  }
  else if (wifi_button == true)
  {
    button_change();
    wifi_button = false;
  }
}
