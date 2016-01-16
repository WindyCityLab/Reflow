#include "Relay.h"
#include "Arduino.h"

#define PERIOD 100 // milliseconds 
#define OFF LOW
#define ON HIGH

Relay::Relay()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  _time = millis();
  _currentState = true;
}

void Relay::setState(bool isOn, float dutyCycle)
{
  _currentState = !isOn;
  _time = millis() + (long)(dutyCycle * PERIOD);
}
void Relay::process()
{
  if (millis() > _time)
  {
    digitalWrite(RELAY_PIN, OFF);
  }
  else
  {
    if (_currentState)
    {
      digitalWrite(RELAY_PIN, ON);
    }
    else
    {
      digitalWrite(RELAY_PIN, OFF);
    }
  }
}


