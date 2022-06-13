#include <pid.hpp>
#define DEBUG

void setupPid(PID_properties & pid, float  set_point, float  kp, float  ki, float  kd)
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

void updatePid(float  actual, PID_properties & present, PID_properties & last)
{
  present.time      = millis();
  present.actual    = actual;
  present.error     = present.set_point - present.actual;

  int dt = present.time-last.time;
  int de = present.error - last.error;
  present.error_div = float(de) / float(dt);

  present.error_sum = (present.error + present.error_sum);
  auto kdp = float(present.error) * present.kp;
  present.output    = kdp + present.error_sum * present.ki + present.kd * present.error_div;
  last              = present;

  #ifdef DEBUG
  Serial.print("A: ");
  Serial.print(present.actual);
  Serial.print("S: ");
  Serial.println(present.set_point);
  // Serial.print(" Error div: ");
  // Serial.print(present.error_div);
  // Serial.print(" Error: ");
  // Serial.print(present.error);
  // Serial.print(" dt: ");
  // Serial.print(dt);
  // Serial.print(" O: ");
  // Serial.println(present.output);
  #endif
}