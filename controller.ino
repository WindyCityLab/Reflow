
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "RingBuffer.h"
#include "Relay.h"

typedef enum {
  Rising, Falling
} RisingFalling;

float currentTemp = 0;
float percentageOverUnder = 0.02; // 2%
long delayTimer = 0;

#define NUMBER_OF_STAGES 10
float setPoints[NUMBER_OF_STAGES] = {150, 160, 170, 180, 190, 200, 225, 240, 220, 0};  //Desire temperature
int times[NUMBER_OF_STAGES] =       {0,    20,  20,  20,  20,  20,   0,  60,  30, 0}; //Time required to stay in this state before transitioning to the next one
RisingFalling direction[NUMBER_OF_STAGES] = {Rising, Rising, Rising, Rising, Rising, Rising, Rising, Rising, Falling, Falling}; //

long holdTimer = 0;

uint8_t stage = 0;

RingBuffer buffer;
Relay relay;

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define DO   3
#define CS   4
#define CLK  5

Adafruit_MAX31855 thermocouple(CLK, CS, DO);

// Example creating a thermocouple instance with hardware SPI (Uno/Mega only)
// on a given CS pin.
//#define CS   10
//Adafruit_MAX31855 thermocouple(CS);

void setup() {
  Serial.begin(9600);

  relay = Relay();
  buffer = RingBuffer();
  //  digitalWrite(RELAY, HIGH); // Remove power
  Serial.println("WCL Oven Initialized...");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.println(thermocouple.readCelsius());
  
  while (1) {
    digitalWrite(RELAY_PIN,LOW);
    delay(2000);
    digitalWrite(RELAY_PIN,HIGH);
    delay(2000);  
  };
}

void loop() {
  relay.process();
  if (Serial.available())
  {
    manageSetPoint();
    Serial.print("Stage "); Serial.print(stage); Serial.println(" Complete.");
    stage++;
    if (stage >= NUMBER_OF_STAGES)
    {
      relay.setState(true,10);
      Serial.print("All Stages Completed, Current Temp: ");
      Serial.println(thermocouple.readCelsius());
      while (1){
        relay.process();
      }
    }
  }
}

bool manageSetPoint()
{
  long timeToExitStage = (times[stage] * 1000) + millis();
  Serial.print("Seeking set point: "); Serial.print(setPoints[stage]); Serial.print(" with delay "); Serial.print(times[stage]); Serial.print(" Exit Time: "); Serial.println(timeToExitStage);

  boolean setPointReached = false;
  delayTimer = millis();

  while (true)
  {
    relay.process();
    if (millis() > delayTimer + 1000)
    {
      delayTimer = millis();
      float value = thermocouple.readCelsius();
      buffer.addToBuffer(value);
      if (buffer.full())
      {
        currentTemp = buffer.average();
        Serial.print(millis()); Serial.print(","); Serial.println(currentTemp);
        if (currentTemp >= setPoints[stage]) {
          relay.setState(false, 10);
          //        digitalWrite(RELAY, HIGH); // Turn off elements
          if (direction[stage] == Rising)
          {
            setPointReached = true;
          }
        }
        else {
          relay.setState(true, 7);
          //        digitalWrite(RELAY, LOW);  // Turn on elements
          if (direction[stage] == Falling)
          {
            setPointReached = true;
          }
        }
        if (setPointReached && timeToExitStage < millis())
        {
          break;
        }
      }
    }
  }
}


