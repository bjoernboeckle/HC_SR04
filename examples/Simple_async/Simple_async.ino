// HC_SR04 - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include "HC_SR04.h"

// ECHO and TRIGGER pin
#define ECHO 2
#define TRIGGER  8

HC_SR04<ECHO> sensor(TRIGGER); // sensor with echo and trigger pin
char buffer[128];              // debug print buffer

// Setup function
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");
  sensor.beginAsync(HC_SR04_ALL);

  // start first measurement
}

// main loop function
void loop()
{
  if (sensor.isFinished())
  {
    sprintf(buffer, "Measured: %ld cm \t", sensor.getDist_cm());
    Serial.println(buffer);
    sensor.startAsync(100000);
  }

  // do something usefull
}

