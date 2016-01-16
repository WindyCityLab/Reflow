
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "RingBuffer.h"
#include "Relay.h"
#include "SimpleTimer.h"

enum States {
  PREHEAT,
  SOAK,
  REFLOW,
  DWELL,
  COOL,
  FINISHED,
  NUMBER_OF_STATES
};

RingBuffer buff;
Relay relay;
SimpleTimer timer;

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define DO   3
#define CS   4
#define CLK  5

bool timeOut = false;

Adafruit_MAX31855 thermocouple(CLK, CS, DO);

typedef struct fsm_struct
{
  float tempTarget;
  float delayTime;
  float pwmRelay;
  bool coilsOn;
  States nextState[2];
} fsm_s;

fsm_s fsm[NUMBER_OF_STATES] = {
  /* PREHEAT */ {150, 100, 0.5, true, {PREHEAT, SOAK}},
  /* SOAK    */ {200, 200, 0.5, true, {SOAK, REFLOW}},
  /* REFLOW  */ {100, 225, 0.5, true, {REFLOW, DWELL}},
  /* DWELL   */ {15, 225, 0.25, true, {DWELL, COOL}},
  /* COOL    */ {300, 0, 0, false, {COOL, FINISHED}},
  /* FINISHED*/ {0, 0, 0, false, {FINISHED, FINISHED}}
};

States currentState = PREHEAT;
int timerID = -1;
uint8_t timeExpired = 0;

void timerExpired()
{
  timeExpired = 1;
  timerID = -1;
}

void printTemp()
{
  Serial.print(thermocouple.readCelsius());
  switch (currentState)
  {
    case PREHEAT : Serial.println(", PREHEAT"); break;
    case SOAK : Serial.println(", SOAK"); break;
    case REFLOW : Serial.println(", REFLOW"); break;
    case DWELL : Serial.println(", DWELL"); break;
    case COOL : Serial.println(", COOL"); break;
    case FINISHED : Serial.println(", FINISHED"); break;
  }
}

void setup() {
  Serial.begin(9600);

  relay = Relay();
  buff = RingBuffer();
  currentState = PREHEAT;
  Serial.println("WCL Oven Initialized...");
  delay(500);
  Serial.println(thermocouple.readCelsius());
  timer.setInterval(1000, printTemp);
}

void handleOutputs()
{
  timeExpired = false;
  if (timerID == -1)
  {
    timerID = timer.setTimeout(fsm[currentState].delayTime, timerExpired);
    relay.setState(fsm[currentState].coilsOn, fsm[currentState].pwmRelay);
  }
}

void loop() {
  relay.process();
  timer.run();

  currentState = fsm[currentState].nextState[timeExpired];
  handleOutputs();

}


