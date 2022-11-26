// HC_SR04_BASE - https://github.com/bjoernboeckle/HC_SR04_BASE
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include "HC_SR04.h"

// define ECHO pins, in case interrupt is not suported, beginAsync will return false
// but the sensor can still be used synchron using startMeasure,
// startAsync will be finisehd immediatly with a distance of 0
#define ECHO_1 2
#define ECHO_2 3
#define ECHO_3 4
#define ECHO_4 5

// trigger pins
#define TRIGGER_1 8
#define TRIGGER_4 11

HC_SR04<ECHO_2> sonicSlaveSharedTrigger1;       // using a 2nd sensor as slave with shared trigger pin
HC_SR04<ECHO_3> sonicSlaveSharedTrigger2;       // using a 3rd sensor as slave with shared trigger pin
HC_SR04<ECHO_4> sonicSlaveTrigger2(TRIGGER_4);  // using a 4thsensor as slave with its own trigger pin

// define master sensor and init with slaves
#define NUM_SLAVES 3
HC_SR04<ECHO_1> sonicMaster(TRIGGER_1, new HC_SR04_BASE *[NUM_SLAVES] { &sonicSlaveSharedTrigger1, &sonicSlaveSharedTrigger2, &sonicSlaveTrigger2 }, NUM_SLAVES);

// debug print buffer
char buffer[128];
void printSensorMeasurements();

// Setup function
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");

  // begin all sensors as asynchron, they can still be used synchron
  // in case one sensor doesn't suport interrupt the function returns false
  sonicMaster.beginAsync(HC_SR04_ALL);
}


// main loop function
void loop()
{
  sonicMaster.startAsync(200000, HC_SR04_ALL);
  while(!sonicMaster.isFinished(HC_SR04_ALL))
  {
    // Do something usefulle while measureing
    // all echo pins which doesnt support interrupt will have a 0 result
  }
  printSensorMeasurements();
  delay(1000);
}



// helper to print measured data for all sensors
void printSensorMeasurements()
{
  Serial.print("Measured: ");
  for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++)
  {
    sprintf(buffer, "%i: %ld cm \t", i, sonicMaster.getDist_cm(i));
    Serial.print(buffer);
  }
  Serial.println();
}
