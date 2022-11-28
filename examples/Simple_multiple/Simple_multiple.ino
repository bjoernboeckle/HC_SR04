// HC_SR04_BASE - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include <HC_SR04.h>

// Sensor / Arduino Pin mapping
//             |  Echo  |  Trigger
// Sensor 1:   |   2    |     8
// Sensor 2:   |   3    |     8
// Sensor 3:   |   4    |     8
// Sensor 4:   |   5    |    11
HC_SR04_BASE *Slaves[] = { new HC_SR04<3>(), new HC_SR04<4>(), new HC_SR04<5>(11) };
HC_SR04<2> sonicMaster(8, Slaves, 3);

// Setup function
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // begin all sensors as asynchron, they can still be used synchron
  // in case one of echo pin doesn't suport interrupt the function returns false
  sonicMaster.begin();
}


// main loop function
void loop()
{
  sonicMaster.startMeasure(200000);
  for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++) {
    Serial.print(sonicMaster.getDist_cm(i));
    Serial.print("  ");
  }
  Serial.println();
  delay(1000);
}


