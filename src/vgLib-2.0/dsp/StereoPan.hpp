#pragma once

struct StereoPan
{
  void process(float *left_voltage, float *right_voltage, float pan)
  {
    if(pan == 0) return;

    if(pan > 0)
    {
      *left_voltage = *left_voltage * (1.0 - pan);
    }
    else // pan < 0
    {
      *right_voltage = *right_voltage * (1.0 + pan);
    }
  }
};
