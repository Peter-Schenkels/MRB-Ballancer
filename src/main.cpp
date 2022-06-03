#include <Arduino.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h> 

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_VL53L0X::VL53L0X_Sense_config_t tof_config = Adafruit_VL53L0X::VL53L0X_Sense_config_t::VL53L0X_SENSE_LONG_RANGE;
Servo servo;

SemaphoreHandle_t range_mutex;
SemaphoreHandle_t angle_mutex;

int current_range = 0;

void TaskReadToF( void *pvParameters );
void TaskSetServo( void *pvParameters );
void TaskUpdatePID( void *pvParameters );
void TaskReadPot( void *pvParameters );
void TaskUpdatOled( void *pvParameters );

void InitHardware()
{
  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  // start continuous ranging 
  if(!lox.configSensor(tof_config))
  {
    Serial.println("Oof");
  }
  lox.startRangeContinuous();
  servo.attach(9);
  
}

void TaskReadToF( void *pvParameters )
{
  // Convert void pointer back to int (delay parameter)
  int delay_ms = *((int*)pvParameters);
  // Start of task
  for (;;)
  {
    // Check if range is accesable by checking its mutex
    if(xSemaphoreTake(range_mutex, 10) == pdTRUE)
    {
      if(lox.isRangeComplete())
      {
        current_range = lox.readRange();
      }
    }
    // Free mutex after accesing the variable
    xSemaphoreGive(range_mutex);
    // Delay and let scheduler pick another task.
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  }
}

void TaskSetServo( void *pvParameters )
{
  // empty  
}

void TaskUpdatOled( void *pvParameters )
{
  // empty  
}

void TaskUpdatePID( void *pvParameters )
{
  // empty  
}

void TaskReadPot( void *pvParameters )
{
  // empty  
}

void InitRtos()
{
  // Create Tasks for scheduler.
  xTaskCreate(TaskReadToF,    "Read Time Of Flight",  128, new int(100),    3, NULL);
  xTaskCreate(TaskSetServo,   "Set servo angle",      128, new int(100),    3, NULL);
  xTaskCreate(TaskUpdatePID,  "Update PID",           128, new int(100),    3, NULL);
  xTaskCreate(TaskReadPot,    "Read Pot Meter",       128, new int(500),    3, NULL);
  xTaskCreate(TaskUpdatOled,  "Update Oled Screen",   128, new int(500),    3, NULL);
  // Create mutexes for shared variables.
  angle_mutex = xSemaphoreCreateMutex();
  range_mutex = xSemaphoreCreateMutex();
}

void setup() {
  Serial.begin(9600);
  // wait until serial port opens for native USB devices
  while (! Serial) {
  }
  InitHardware();
  InitRtos(); 


}




void loop()
{
    // Empty
}



