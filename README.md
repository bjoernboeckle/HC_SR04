# HC_SR04

This repository contains a library that can be used with the [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32). It supports asyncron and synchron readout of HC-SR04 sensors also supporting multiple sensors using the same trigger.

## Docu

### Simple example (synchron)
Use synchron readout, startMeasure blocks further program execution.
Result is available after sensor."startMeasure();" returns.

    #include <Arduino.h>
    #include <HC_SR04.h>

    HC_SR04<2> sensor(8); // sensor with echo pin 2 (interrupt) and trigger pin 8

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

    // main loop function
    void loop()
    {
      if (sensor.isFinished()) {
        Serial.println(sensor.getDist_cm());
        sensor.startAsync();
      }
      // do something usefull while measurement is running
    }
