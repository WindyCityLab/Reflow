#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "Relay.h"
#include "SimpleTimer.h"

enum States {
  PREHEAT,
  SOAK,
  SOAK2,
  REFLOW,
  DWELL,
  COOL,
  FINISHED,
  NUMBER_OF_STATES
};

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
  float targetTemperature;
  float timeInState;
  States nextState[2];
} fsm_s;

fsm_s fsm[NUMBER_OF_STATES] = {
  /* PREHEAT */ {165, 90, {PREHEAT, SOAK}},
  /* SOAK    */ {200, 90, {SOAK, SOAK2}},
  /* SOAK2   */ {245, 30, {SOAK2, REFLOW}},
  /* REFLOW  */ {290, 30, {REFLOW, DWELL}},
  /* DWELL   */ {245, 30, {DWELL, COOL}},
  /* COOL    */ {0, 0, {COOL, FINISHED}},
  /* FINISHED*/ {0, 0, {FINISHED, FINISHED}}
};

States currentState = PREHEAT;
int timerID = -1;
uint8_t timeExpired = 0;
volatile float currentTemp = 0;
volatile bool readTemp = false;
void timerExpired()
{
  timeExpired = 1;
  timerID = -1;
}

void printTemp()
{
  readTemp = true;
  Serial.print(fsm[currentState].targetTemperature);
  Serial.print(", ");
  Serial.print(currentTemp);
  Serial.print(", ");
  Serial.print(relay.currentDutyCycle());
  Serial.print(", ");
  Serial.print(relay.reset);
  switch (currentState)
  {
    case PREHEAT : Serial.println(", PREHEAT"); break;
    case SOAK : Serial.println(", SOAK"); break;
    case SOAK2 : Serial.println(", SOAK2"); break;
    case REFLOW : Serial.println(", REFLOW"); break;
    case DWELL : Serial.println(", DWELL"); break;
    case COOL : Serial.println(", COOL"); break;
    case FINISHED : Serial.println(", FINISHED"); break;
  }
}

void setup() {
  Serial.begin(9600);

  relay = Relay();
  currentState = PREHEAT;
  Serial.print("WCL Oven Initialized... Current temp: ");
  currentTemp = thermocouple.readCelsius();
  Serial.println(currentTemp);
}

void handleOutputs()
{
  timeExpired = false;
  if (timerID == -1)
  {
    timerID = timer.setTimeout(fsm[currentState].timeInState * 1000, timerExpired);
    relay.setTargetTemp(fsm[currentState].targetTemperature);
  }
}

void loop() {
  while (!Serial.available());
  timer.setInterval(1000, printTemp);

  Serial.println("Starting Reflow...");
  while (1) {
    if (readTemp)
    {
      currentTemp = thermocouple.readCelsius();
      readTemp = false;
    }

    relay.process(currentTemp);
    timer.run();

    currentState = fsm[currentState].nextState[timeExpired];
    handleOutputs();
  }
}


