#include <Arduino.h>
#pragma once
class StepEncoder
{
  int64_t old_val = 0;

public:
  int8_t get_current_diff();
};