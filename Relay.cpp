#include "Relay.h"
#include "Arduino.h"

Relay::Relay()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  _time = millis();
  _currentState = true;
  _dutyCycle = 10;
  _counter = 0;
}

void Relay::setState(bool isOn, int dutyCycle)
{
  _currentState = !isOn;
  _dutyCycle = dutyCycle;
  _counter = 0;
}

void Relay::process()
{
  if (millis() > _time + 100)
  {
    if (_counter < _dutyCycle)
    {
      digitalWrite(RELAY_PIN, _currentState);
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);
    }
    _counter = (_counter + 1) % 10;
    _time = millis();
  }
}


