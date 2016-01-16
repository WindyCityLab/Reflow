#ifndef RELAY
#define RELAY

#define RELAY_PIN 7

class Relay
{
  public:
    Relay();
    void setState(bool isOn, float dutyCycle);
    void process();
    
  private:
    void timeExpired();
    bool _currentState;
    long _time;
};

#endif
