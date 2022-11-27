# HC_SR04

This repository contains a library that can be used with the [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32). It supports asyncron and synchron readout of HC-SR04 sensors also supporting multiple sensors using the same trigger.

## Docu

### Simple example (synchron)
Use synchron readout, startMeasure blocks further program execution.
Result is available after sensor."startMeasure();" returns.

    #include <Arduino.h>
    #include <HC_SR04.h>

    HC_SR04<2> sensor(8); // sensor with echo pin 2 (interrupt not required) and trigger pin 8

    void setup() {Serial.begin(9600); sensor.begin(); }

    void loop() {
      sensor.startMeasure();
      Serial.println(sensor.getDist_cm());
    }

### Simple example (asynchron)
Use asynchron readout, startAsync only starts a measurement, other tasks can be performed while measurement is running.
Use sensor.isFinished() to check if data is available.

    #include <Arduino.h>
    #include <HC_SR04.h>

    HC_SR04<2> sensor(8); // sensor with echo pin 2 (interrupt) and trigger pin 8

    void setup() {Serial.begin(9600); sensor.beginAsync(); }

    void loop()
    {
      if (sensor.isFinished()) {
        Serial.println(sensor.getDist_cm());
        sensor.startAsync();
      }
      // do something usefull while measurement is running
    }

### Multi sensor example (synchron)
Use four sensors with synchron readout, startMeasure blocks further program execution.
Result is available after sensor."startMeasure();" returns. Time required is the time 
to measure all four sensors.

    #include <Arduino.h>
    #include <HC_SR04.h>

    HC_SR04<3> sonicSlaveSharedTrigger1;     // using a 2nd sensor echo pin 3 as slave with shared trigger pin (no pin provided)
    HC_SR04<4> sonicSlaveSharedTrigger2;     // using a 3rd sensor pin 4 as slave with shared trigger pin (no pin provided)
    HC_SR04<5> sonicSlaveTrigger2(9);        // using a 4th sensor pin 5 as slave with its own trigger pin 9

    // define master sensor echo pin 2, trigger pin 8 and init with slaves
    HC_SR04<2> sonicMaster(8, new HC_SR04_BASE *[3] { &sonicSlaveSharedTrigger1, &sonicSlaveSharedTrigger2, &sonicSlaveTrigger2 }, 3);

    void setup() { Serial.begin(9600); sonicMaster.begin(HC_SR04_ALL); }

    void loop() {
      sonicMaster.startMeasure(200000, HC_SR04_ALL);
      for(int i=0; i<sonicMaster.getNumberOfSensors(); i++ ){
        Serial.print(sonicMaster.getDist_cm());
        Serial.println(" cm ");
      }
    }


