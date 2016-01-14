#ifndef RELAY
#define RELAY

#define RELAY_PIN 7
 
class Relay
{
  public:
    Relay();
    void setState(bool isOn, int dutyCycle);
    void process();
    
  private:
    bool _currentState;
    int _dutyCycle;
    int _counter;
    long _time;
};

#endif
