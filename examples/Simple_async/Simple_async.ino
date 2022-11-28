// HC_SR04 - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include <HC_SR04.h>

HC_SR04<2> sensor(8);   // sensor with echo and trigger pin

void setup() { 
  Serial.begin(9600); 
  sensor.beginAsync();  
  sensor.startAsync(100000);        // start first measurement
}

// main loop function
void loop() {
  if (sensor.isFinished()) {
    Serial.println(sensor.getDist_cm());
    delay(1000);

    sensor.startAsync(100000);
  }

  // do something usefull
}

