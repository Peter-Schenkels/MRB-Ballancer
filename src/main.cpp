#include <Arduino.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"
#include <FreeRTOS.h>
#include <semphr.h> 
#include <Adafruit_SSD1306.h>
#include <queue.h>
#include <pid.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define ever ;;
#define WIPWAP_LENGTH 350
#define SECOND_MS 1000
#define MAX_SERVO 45
#define MIN_SERVO 10
// #define DEBUG

Servo servo;

int pot_pin = A3;
int pot_value = 0;

float P = 2;
float I = 0.003;
float D  = 0.2;

SemaphoreHandle_t range_mutex = NULL;
SemaphoreHandle_t angle_mutex = NULL;
SemaphoreHandle_t pot_value_mutex = NULL;
QueueHandle_t range_queue = NULL;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int current_range = 0;
int current_angle = 0;
int set_point_angle = (MAX_SERVO + MIN_SERVO) / 2 +150;
int servo_speed_degree_per_second = 5;
int range_error;

PID_properties present_pid;
PID_properties last_pid;

void TaskReadToF( void *pvParameters );
void TaskSetServo( void *pvParameters );
void TaskUpdatePID( void *pvParameters );
void TaskReadPot( void *pvParameters );
void TaskUpdateOled( void *pvParameters );

void InitTof()
{
  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) 
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1); // Failed
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_Sense_config_t::VL53L0X_SENSE_LONG_RANGE);
  lox.startRangeContinuous();
}

void InitOled()
{
  //Init Oled 128x32
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // Failed
  }
}

void InitHardware()
{
  InitTof();
  InitOled();
  servo.attach(9);    // Attach servo unit to pin 9
  delay(2000);        // Pause for 2 seconds
}

void TaskReadToF( void *pvParameters )
{
  // Convert void pointer parameter to delay in ms
  int delay_ms = (int)pvParameters;
  // Start of task
  while(1)
  {
      if(lox.isRangeComplete())
      {
        uint16_t read_range = lox.readRange();
        xQueueSend(range_queue, &read_range, portMAX_DELAY);
      }
    // Delay and let scheduler pick another task.
    vTaskDelay(delay_ms);
    }
}

void TaskSetServo( void *pvParameters )
{
  int task_length = (int)pvParameters;
  for(ever)
  {
    if(xSemaphoreTake(angle_mutex, 10) == pdTRUE)
    {
      if(current_angle != set_point_angle)
      {
        int speed = servo_speed_degree_per_second * ((float)task_length/(float)SECOND_MS);
        speed = current_angle > set_point_angle ? speed : speed*-1;
        current_angle = abs(speed + current_angle - set_point_angle) > abs(speed) ?  set_point_angle : current_angle + speed;
        #ifdef DEBUG
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "DPS:%u, SPEED:%d, ANGLE:%u, SET_POINT:%u", servo_speed_degree_per_second, speed, current_angle, set_point_angle);
        Serial.println(buffer);
        #endif // DEBUG
        if(current_angle < MAX_SERVO && current_angle > MIN_SERVO)
        {
          servo.write(current_angle);
        }
      }
      xSemaphoreGive(angle_mutex);
    }
    vTaskDelay(task_length);
  }
}

void TaskUpdateOled( void *pvParameters )
{
  for(ever)
  {
    if(xSemaphoreTake(range_mutex, 10) == pdTRUE)
    {
      display.setTextSize(1);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(0, 0);     // Start at top-left corner
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      display.clearDisplay();
      char buffer[20];
      snprintf(buffer, sizeof(buffer), "range:%d", current_range);
      display.println(buffer);
      snprintf(buffer, sizeof(buffer), "P:%f, I:%f, D%f", P, I, D);
      display.println(buffer);
      xSemaphoreGive(range_mutex);
    }
    vTaskDelay(300);
  }
}

uint8_t postProcessSteeringAction(float steering_action)
{
  return ((steering_action + UINT8_MAX)/UINT8_MAX) * (MAX_SERVO - MIN_SERVO) + MIN_SERVO;
}


void TaskUpdatePID( void *pvParameters )
{
  int delay_ms = (int)pvParameters;
  uint16_t que_item = 0;
  uint16_t total_range = 0;
  uint8_t total_queue_items = 0;
  for(ever)
  {
    // Read all range items from queue and add them to each other to get a mean value.
    while(xQueueIsQueueEmptyFromISR(range_queue) == pdFALSE)
    {
      if(xQueueReceive(range_queue, &que_item, portMAX_DELAY) == pdPASS)
      {
        total_range += que_item;
        total_queue_items++;
      }
    }
    // Check if range is accesable by checking its mutex
    if(xSemaphoreTake(range_mutex, 10) == pdTRUE)
    {
      // Get mean value from read queue items.
      current_range = total_range/total_queue_items;
      // Update the PID
      if(current_range != 0)
      {
        current_range = (current_range - range_error) < 0 ? 0 : current_range-range_error;
        updatePid(current_range, present_pid, last_pid);
        int angle = postProcessSteeringAction(present_pid.output);
        if(xSemaphoreTake(angle_mutex, 10) == pdTRUE)
        {
          set_point_angle = angle;
          xSemaphoreGive(angle_mutex);
        }
      }
      // Reset read values.
      total_range = 0;
      total_queue_items = 0;
      // Free mutex after accesing the variable
      xSemaphoreGive(range_mutex);
    }
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  }
}

void TaskReadPot( void *pvParameters )
{
  // Convert void pointer back to int (delay parameter)
  int delay_ms = (int)pvParameters;
  // Start of task
  for(ever)
  {
    // if(xSemaphoreTake(pot_value_mutex, 10) == pdTRUE)
    // {
    //   // pot_value = WIPWAP_LENGTH*UINT8_MAX/analogRead(pot_pin);
    Serial.println(analogRead(pot_pin));
    //100
    present_pid.kp = float(analogRead(pot_pin))/400.f * -1;
    //   xSemaphoreGive(pot_value_mutex);
    // }
    // Delay and let scheduler pick another task.
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  }
}

void InitRtos()
{
  // Create queues for ranges.
  range_queue = xQueueCreate(10, sizeof(int));
  // Create mutexes for shared variables.
  angle_mutex = xSemaphoreCreateMutex();
  range_mutex = xSemaphoreCreateMutex();
  // Confirm is mutexes were created.
  if(angle_mutex == NULL || range_mutex == NULL)
  {
    Serial.println("Failed to init mutexes");
  }
  // Create Tasks for scheduler.
  xTaskCreate(TaskReadToF,    "Read Time Of Flight",  4028, (void*)10 ,    2, NULL);
  xTaskCreate(TaskSetServo,   "Set servo angle",      1048, (void*)100,    3, NULL);
  xTaskCreate(TaskUpdatePID,  "Update PID",           1048, (void*)100,    3, NULL);
  xTaskCreate(TaskReadPot,    "Read Pot Meter",       1028, (void*)500,    2, NULL);
  xTaskCreate(TaskUpdateOled,  "Update Oled Screen",  4028, (void*)500,    2, NULL);
}

void setup() {
  // wait until serial port opens for native USB devices
  Serial.begin(9600);
  while (! Serial){}
  // Setup PID controller
  setupPid(present_pid, WIPWAP_LENGTH/2, P, I, D);
  setupPid(last_pid, WIPWAP_LENGTH/2, P, I, D);
  InitHardware();
  InitRtos(); 
	// Start the RTOS kernel
	vTaskStartScheduler();
  // out of memory
  Serial.println("Insufficient RAM");
}

void loop()
{
}



