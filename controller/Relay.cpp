#include "Relay.h"
#include "Arduino.h"

#define PERIOD 100.0 // milliseconds 
#define K (2.0 / 100.0)
#define tau_i 25000.0
#define MAX_DUTY_CYCLE 1.0

#define OFF LOW
#define ON HIGH

Relay::Relay()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  _currentState = OFF;
  _timeToFlip = 0;
  _dutyCycle = 0.5;
  reset = 0.0;
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
  float error = _desiredTemp - currentTemp;
  reset = max(min(reset + (K / tau_i) * error,MAX_DUTY_CYCLE),0);
  reset = 0; // Disable the I component for now
  float output =  K * error + reset;
  if (output < 0)
  {
    _dutyCycle = 0;
  }
  else
  {
    _dutyCycle = min(output, MAX_DUTY_CYCLE);
  }
  
  if (millis() > _timeToFlip)
  {
    if (_currentState == ON)
    {
      _currentState = OFF;
      _timeToFlip = millis() + (unsigned long)((1.0 - _dutyCycle) * PERIOD);
    }
    else
    {
      _currentState = ON;
      _timeToFlip = millis() + (unsigned long)(_dutyCycle * PERIOD);

    }
  }
  if (_dutyCycle == 0) digitalWrite(RELAY_PIN, LOW);
  else digitalWrite(RELAY_PIN, _currentState);
}


