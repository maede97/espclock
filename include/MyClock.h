#pragma once

#include <Arduino.h>

class MyClock
{
public:
  MyClock()
  {
    hours = 0;
    minutes = 0;
    seconds = 0;
  }

  void setHours(char value)
  {
    hours = value;
  }

  void setMinutes(char value)
  {
    minutes = value;
  }

  void setSeconds(char value)
  {
    seconds = value;
  }

  char getHours()
  {
    return hours;
  }

  char getMinutes()
  {
    return minutes;
  }

  char getSeconds()
  {
    return seconds;
  }

  std::tuple<bool, bool, bool> update()
  {
    bool updatedS = false;
    bool updatedM = false;
    bool updatedH = false;

    unsigned long now = millis();
    if (now - lastUpdate >= 1000)
    {
      updatedS = true;
      lastUpdate = now;
      if (seconds < 59)
      {
        seconds++;
      }
      else
      {
        seconds = 0;
        updatedM = true;
        if (minutes < 59)
        {
          minutes++;
        }
        else
        {
          minutes = 0;
          updatedH = true;
          if (hours < 23)
          {
            hours++;
          }
          else
          {
            hours = 0;
          }
        }
      }
    }
    return {updatedS, updatedM, updatedH};
  }

private:
  char hours;
  char minutes;
  char seconds;
  unsigned long lastUpdate;
};
