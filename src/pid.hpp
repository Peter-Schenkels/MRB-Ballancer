#ifndef __PID_HPP__
#define __PID_HPP__

#include <Arduino.h>
#include <Wire.h>

struct PID_properties
{
  float set_point;
  float actual;
  float kp;
  float ki;
  float kd;
  float error;
  float error_sum;
  float error_div;
  float output;
  uint32_t time;
};

void setupPid(PID_properties & pid, float set_point, float kp, float ki, float kd);

void updatePid(uint16_t actual, PID_properties & present, PID_properties & last);

#endif