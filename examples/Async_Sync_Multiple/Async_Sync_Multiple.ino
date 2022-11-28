// HC_SR04_BASE - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#include <Arduino.h>
#include <HC_SR04.h>

// define ECHO pins, in case interrupt is not suported, beginAsync will return false
// but the sensor can still be used synchron using startMeasure,
// startAsync will be finisehd immediatly with a distance of 0

// Sensor / Arduino Pin mapping
//             |  Echo  |  Trigger
// Sensor 1:   |   2    |     8
// Sensor 2:   |   3    |     8
// Sensor 3:   |   4    |     8
// Sensor 4:   |   5    |    11
HC_SR04_BASE *Slaves[] = { new HC_SR04<3>(), new HC_SR04<4>(), new HC_SR04<5>(11) };
HC_SR04<2> sonicMaster(8, Slaves, 3);

// debug print buffer
char buffer[128];
void printSensorMeasurements();

// Setup function
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting up...");

  // begin all sensors as asynchron, they can still be used synchron
  // in case one sensor doesn't suport interrupt the function returns false
  sonicMaster.beginAsync();

  // show an error message for all sensors which doesn't support  interrupt
  for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++)
  {
    if (!sonicMaster.isInterruptSupported(i))
    {
      sprintf(buffer, "Sensor, %d: *****FAILED Interrupt!!!!!!!!", i);
      Serial.println(buffer);
    }
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
  // also measurements are executed paralell, much faster then synchron
  // in case no pulse was measured, measurement is stopped after timeout (100ms)
  start = micros();
  sonicMaster.startAsync(200000);
  while (!sonicMaster.isFinished() )
  {
    // do something usefull while measurement is running
  }
  duration = micros() - start;

  sprintf(buffer, "Async time: %ld us\t", duration);
  Serial.print(buffer);
  printSensorMeasurements();
  // --------------------------------------------------------

  delay(500);

  // --------------------------------------------------------
  // perform a synchronous measurement, all sensors can measure data also sensor which doesn't support interrupt 
  // no work can be done while measurmeent is runnig
  // also measurements are executed sequentiell, longer measure time
  start = micros();
  sonicMaster.startMeasure(200000);
  duration = micros() - start;

  sprintf(buffer, "Sync  time: %ld us\t", duration);
  Serial.print(buffer);
  printSensorMeasurements();
  // --------------------------------------------------------


  delay(500);
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
