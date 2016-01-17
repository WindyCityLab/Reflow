#include "Relay.h"
#include "Arduino.h"

#define PERIOD 100.0 // milliseconds 
#define GAIN 2.0

#define OFF LOW
#define ON HIGH

Relay::Relay()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  _currentState = OFF;
  _timeToFlip = 0;
  _dutyCycle = 0.5;
}

void Relay::setTargetTemp(float desiredTemp)
{
  _desiredTemp = desiredTemp;
}
float Relay::currentDutyCycle()
{
  return _dutyCycle;
}
void Relay::process(float currentTemp)
{
  if (_desiredTemp < currentTemp) _dutyCycle = 0;
  else _dutyCycle = min(((_desiredTemp - currentTemp) / 100.0) * GAIN,0.9);
  
  if (millis() > _timeToFlip)
  {
    if (_currentState == ON)
    {
      _currentState = OFF;
      _timeToFlip = millis() + (unsigned long)((1.0 - _dutyCycle) * PERIOD);
//      Serial.print("Now LOW, current Time: "); Serial.print(millis()); Serial.print("  Turning HIGH at: "); Serial.println(_timeToFlip);
    }
    else
    {
      _currentState = ON;
      _timeToFlip = millis() + (unsigned long)(_dutyCycle * PERIOD);
//      Serial.print("Now HIGH, current Time: "); Serial.print(millis()); Serial.print("  Turning LOW at: "); Serial.println(_timeToFlip);

    }
  }
  if (_dutyCycle == 0) digitalWrite(RELAY_PIN, LOW);
  else digitalWrite(RELAY_PIN, _currentState);
}


