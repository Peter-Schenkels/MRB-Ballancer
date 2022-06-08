#include <pid.hpp>
#define DEBUG

void setupPid(PID_properties & pid, float set_point, float kp, float ki, float kd)
{
  pid.set_point = set_point;
  pid.kp        = kp;
  pid.ki        = ki;
  pid.kd        = kd;
  pid.actual    = 0;
  pid.error_div = 0;
  pid.error     = 0;
  pid.error_sum = 0;
  pid.output    = 0;
  pid.time      = millis();
}

void updatePid(uint16_t actual, PID_properties & present, PID_properties & last)
{
  present.time      = millis();
  present.actual    = actual;
  present.error     = present.set_point - present.actual;

  float dt = float(present.time-last.time);
  present.error_div = (present.error - last.error) / dt;

  present.error_sum = (present.error + present.error_sum);
  present.output    = present.error * present.kp + present.error_sum * present.ki + present.kd * present.error_div;
  last              = present;

  #ifdef DEBUG
  Serial.print("Set Point: ");
  Serial.print(present.set_point);
  Serial.print(" Actual: ");
  Serial.print(present.actual);
  Serial.print(" Error div: ");
  Serial.print(present.error_div);
  Serial.print(" Error: ");
  Serial.print(present.error);
  Serial.print(" Error sum: ");
  Serial.print(present.error_sum);
  Serial.print(" Output: ");
  Serial.println(present.output);
  #endif
}