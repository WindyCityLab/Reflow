#ifndef RELAY
#define RELAY

#define RELAY_PIN 7
//#define RELAY_PIN 13

class Relay
{
  public:
    Relay();
    void setTargetTemp(float desiredTemp);
    void process(float currentTemp);
    float currentDutyCycle();
    
  private:
    bool _currentState;
    float _dutyCycle;
    bool _shouldBeOn;
    unsigned long _timeToFlip;
    float _desiredTemp;
};

#endif
