// HC_SR04 - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include "HC_SR04.h"

// define ECHO pins in case interrupt is not suported, beginAsync will return false
// but the sensor can still be used synchron using startMeasure,
// startAsync will be finisehd immediatly with a distance of 0
#define ECHO 2
#define TRIGGER  8


HC_SR04<ECHO> sensor(TRIGGER);


// debug print buffer
char buffer[128];
void printSensorMeasurement();


// Setup function
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");

  sensor.beginAsync(HC_SR04_ALL);
   // show an error message in case sensor doesn't support interrupt
  if (!sensor.isInterruptSupported())
  {
    sprintf(buffer, "Sensor: *****FAILED Interrupt!!!!!!!!");
    Serial.println(buffer);
  }
}


// main loop function
void loop()
{
  unsigned long start = 0;
  unsigned long duration = 0;

  // --------------------------------------------------------
  // perform a asynchronous measurement, sensor which doesn't support interrupt will have a 0 result
  // work can be done while measurmeent is runnig
  // in case no pulse was measured, timeout will be elapsed (100ms)
  start = micros();
  sensor.startAsync(100000);
  while (!sensor.isFinished() )
  {
    // do something usefull while measurement is running
  }
  duration = micros() - start;

  sprintf(buffer, "Async time: %ld us\t", duration);
  Serial.print(buffer);
  printSensorMeasurement();
  // --------------------------------------------------------

  delay(500);

  // --------------------------------------------------------
  // perform a synchronous measurement, all sensors can measure data also sensor which doesn't support interrupt 
  // no work can be done while measurmeent is runnig
  start = micros();
  sensor.startMeasure();
  duration = micros() - start;

  sprintf(buffer, "Sync  time: %ld us\t", duration);
  Serial.print(buffer);
  printSensorMeasurement();
  // --------------------------------------------------------


  delay(500);
}



// helper to print measured data for all sensors
void printSensorMeasurement()
{
  sprintf(buffer, "Measured: %ld cm \t", sensor.getDist_cm());
  Serial.print(buffer);
  Serial.println();
}
