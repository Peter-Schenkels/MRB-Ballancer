#include <Arduino.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"
#include <FreeRTOS.h>
#include <semphr.h> 
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Servo servo;

int pot_pin = A3;
int pot_value = 0;

SemaphoreHandle_t range_mutex = NULL;
SemaphoreHandle_t angle_mutex = NULL;
SemaphoreHandle_t pot_value_mutex = NULL;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int current_range = 0;
const uint16_t max_range = 320;

void TaskReadToF( void *pvParameters );
void TaskSetServo( void *pvParameters );
void TaskUpdatePID( void *pvParameters );
 
 void TaskReadPot( void *pvParameters );
void TaskUpdateOled( void *pvParameters );

void InitHardware()
{
  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) 
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1); // Failed
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_Sense_config_t::VL53L0X_SENSE_LONG_RANGE);
  lox.startRangeContinuous();
  // Attach servo unit to pin 9
  servo.attach(9);
  //Init Oled 128x32
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // Failed
  }
  display.display(); // Draw logo
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.cp437(true);   
  display.setTextColor(SSD1306_WHITE);
  delay(2000); // Pause for 2 seconds
}

void TaskReadToF( void *pvParameters )
{
  // Convert void pointer parameter to delay in ms
  int delay_ms = *((int*)pvParameters);
  
  // Start of task
  while(1)
  {
    // Check if range is accesable by checking its mutex
    if(xSemaphoreTake(range_mutex, 10) == pdTRUE)
    {

      if(lox.isRangeComplete())
      {
        current_range = lox.readRange();
        Serial.println(current_range);
      }
      // Free mutex after accesing the variable
      xSemaphoreGive(range_mutex);
    }
    // Delay and let scheduler pick another task.
    vTaskDelay(100);
  }
}

void TaskSetServo( void *pvParameters )
{
  // empty  
}

void TaskUpdateOled( void *pvParameters )
{
  while(1)
  {
    if(xSemaphoreTake(range_mutex, 10) == pdTRUE)
    {
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(0, 0);     // Start at top-left corner
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      display.clearDisplay();
      char buffer[20];
      sprintf(buffer, "Range:%d", current_range);
      for(uint8_t i = 0; i<20; i++)
      {
        display.write(buffer[i]);
      }    
      display.display();
      xSemaphoreGive(range_mutex);
    }
    vTaskDelay(300);
  }
}

void TaskUpdatePID( void *pvParameters )
{
  // empty  
}

void TaskReadPot( void *pvParameters )
{
  // Convert void pointer back to int (delay parameter)
  int delay_ms = *((int*)pvParameters);
  // Set bool for if task
  bool task_crash = false;
  // Start of task
  while(!task_crash)
  {
    if(xSemaphoreTake(pot_value_mutex, 10) == pdTRUE)
    {
      pot_value = max_range*UINT8_MAX/analogRead(pot_pin);
      xSemaphoreGive(pot_value_mutex);
    }
    // Delay and let scheduler pick another task.
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  }
  free(pvParameters);
}

void InitRtos()
{
  // Create mutexes for shared variables.
  angle_mutex = xSemaphoreCreateMutex();
  range_mutex = xSemaphoreCreateMutex();
  if(angle_mutex == NULL || range_mutex == NULL)
  {
    Serial.println("Failed to init mutexes");
  }
  // Create Tasks for scheduler.
  xTaskCreate(TaskReadToF,    "Read Time Of Flight",    4028, (void *)100,    2, NULL);
  // xTaskCreate(TaskSetServo,   "Set servo angle",      128, new int(100),    3, NULL);
  // xTaskCreate(TaskUpdatePID,  "Update PID",           128, new int(100),    3, NULL);
  // xTaskCreate(TaskReadPot,    "Read Pot Meter",       128, new int(500),    3, NULL);
  xTaskCreate(TaskUpdateOled,  "Update Oled Screen",   4028, (void *)500,    2, NULL);
}

void setup() {
  Serial.begin(9600);
  // wait until serial port opens for native USB devices
  while (! Serial) {
  }
  InitHardware();
  InitRtos(); 

	// Start the RTOS kernel
	vTaskStartScheduler();

  // out of memory
  Serial.println("Insufficient RAM");
}




void loop()
{
  // delay(1);
}



