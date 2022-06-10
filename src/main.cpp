#include <Arduino.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"
#include <pid.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define ever ;;
#define WIPWAP_LENGTH 250
#define SECOND_MS 1000
#define MAX_SERVO 1100
#define MIN_SERVO 700
// #define DEBUG

Servo servo;

int pot_pin = A3;
int pot_value = 0;

float P = 2;
float I = 0;
float D = 0;

PID_properties present_pid;
PID_properties last_pid;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // wait until serial port opens for native USB devices
  Serial.begin(9600);
  while (! Serial){}
  servo.writeMicroseconds((MIN_SERVO + MAX_SERVO)/2);
  servo.attach(9);    // Attach servo unit to pin 9
  setupPid(present_pid, WIPWAP_LENGTH/2, P, I, D);
  setupPid(last_pid, WIPWAP_LENGTH/2, P, I, D);
  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) 
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1); // Failed
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_Sense_config_t::VL53L0X_SENSE_LONG_RANGE);
  lox.startRangeContinuous();
}

void loop()
{
  if(lox.isRangeComplete())
  {
    int current_range = lox.readRange();
    updatePid(current_range, present_pid, last_pid);
    int angle = map(present_pid.output, -55, 55, MIN_SERVO, MAX_SERVO);
    if(angle > MIN_SERVO && angle < MAX_SERVO)
    {
      servo.writeMicroseconds(angle);
    }
  }
}



