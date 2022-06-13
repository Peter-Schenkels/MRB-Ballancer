#include <Arduino.h>
#include <pid.hpp>
#include <Servo.h>
#include <Ultrasonic.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define WIPWAP_LENGTH 250
#define MAX_SERVO 2130
#define MIN_SERVO 900
#define pl Serial.println
// #define DEBUG

Servo servo;

uint8_t pot_pin = A3;
uint16_t pot_value = 0;
Ultrasonic ultrasonic(12, 13);

float P =  2;
int I = 0;
int D = 500;

PID_properties present_pid;
PID_properties last_pid;

void ServoTest();

void TofTest();

void setup() {
  // wait until serial port opens for native USB devices
  Serial.begin(9600);
  while (! Serial){}
  digitalWrite(SDA, 0);
  digitalWrite(SCL, 0);
  servo.writeMicroseconds((MIN_SERVO + MAX_SERVO)/2);
  servo.attach(9);    // Attach servo unit to pin 9
  setupPid(present_pid, WIPWAP_LENGTH/2, P, I, D);
  setupPid(last_pid, WIPWAP_LENGTH/2, P, I, D);
}

void TofTest()
{
  while(1)
  {
    auto start = millis();
    auto range = ultrasonic.read();
    auto done = millis() - start;
    pl(range);
  }
}

void ServoTest()
{
  while(1)
  {
    for(uint16_t i = MIN_SERVO; i  < MAX_SERVO; i+=5)
    {
      servo.writeMicroseconds(i);
      pl(i);
      delay(10);
    }
        for(uint16_t i = MAX_SERVO; i  !=  MIN_SERVO; i-=5)
    {
      servo.writeMicroseconds(i);
      pl(i);
      delay(10);
    }
  }
}

uint16_t collectReadings(uint16_t time_ms)
{
  uint16_t total_range = 0;
  uint16_t nr_of_readings = 0;
  uint32_t start = millis();
  for(; start+time_ms > millis(); nr_of_readings++)
  {
    total_range += ultrasonic.read() * 10;
  }
  return total_range / nr_of_readings;
}

void loop()
{
  uint16_t current_range = collectReadings(100);
  updatePid(current_range, present_pid, last_pid);
  present_pid.set_point = map(analogRead(pot_pin), 0, 1024, 30, 270);
  int angle = map(present_pid.output, -1000, 1000, MIN_SERVO, MAX_SERVO);
  servo.writeMicroseconds(angle);
}



