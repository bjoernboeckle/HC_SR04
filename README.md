# HC_SR04

This repository contains a library that can be used with Arduino and ESP32. It supports asyncron and synchron readout of HC-SR04 sensors also supporting multiple sensors using the same trigger.

In case asynchron is used the readout runs paralell, while synchron runs sequential. Therefore in case more than one sensor is used the asynchron version is much faster compared to sequential.

Also in asynchron the microcontroller can perform other work while the measurement is running.

# Quick start

## Simple example (synchron)

Use synchron readout, "startMeasure" blocks further program execution.
Result is available after sensor."startMeasure();" returns.

    #include <Arduino.h>
    #include <HC_SR04.h>

    HC_SR04<2> sensor(8);   // sensor with echo and trigger pin

    void setup() {  Serial.begin(9600);  sensor.begin(); }

    void loop() {
        sensor.startMeasure();
        Serial.println(sensor.getDist_cm());
        delay(1000);
    }


# Usage

Initialize with echo pin (2) and trigger pin (8). In case asynchron measuremnt should be used the echo pin must support interrupts.

    HC_SR04<2> sensor(8);

Initialize hardware, set pin modes etc. IN case of asynchron readout also attach interrupts.

    sensor.begin();

    or

    sensor.beginAsync();  // for asynchron

Start a measurement. Synchron will block the cpu until the measurement is finshed. Async returns immediately, "isFinshed" can be used to chek if measurmeent is finished while performing other tasks.

    sensor.startMeasure();

    or

    sensor.startAsync();    // for asynchron


Check if a result is available.
    
    bool isFinished()

After the measurement is finshed the result can be read by using getDistance functions in mm, cm and inches.

    long getDist_cm()
    long getDist_mm()
    long getDist_inch()


Release hardware (pin modes etc.), in case of asynchorn also detach interrupts.

    release();


# Methods

    bool begin(int slave = HC_SR04_ALL);
    bool startMeasure(unsigned long timeout = 0, int slave = HC_SR04_ALL);

    bool beginAsync(int slave = HC_SR04_ALL);
    bool startAsync(unsigned long timeout = 0, int slave = HC_SR04_ALL );
    bool isFinished(int slave = HC_SR04_ALL);

    unsigned long getDuration_us(int slave = HC_SR04_MASTER);

    long getDist_cm(int slave = HC_SR04_MASTER);
    long getDist_mm(int slave = HC_SR04_MASTER);
    long getDist_inch(int slave = HC_SR04_MASTER);

    bool HasTrigger(int slave = HC_SR04_MASTER);
    int getNumberOfSensors();
    HCSR04_State getState(int slave = HC_SR04_MASTER);
    bool isInterruptSupported(int slave = HC_SR04_MASTER);
    
    void release(int slave = HC_SR04_ALL);

# Examples

## Simple example (asynchron)
Use asynchron readout, startAsync only starts a measurement, other tasks can be performed while measurement is running.
Use sensor.isFinished() to check if data is available.

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

## Multi sensor example (synchron)
Use four sensors with synchron readout, "startMeasure" blocks further program execution.
Result is available after sensor."startMeasure();" returns. Time required is the time 
to measure all four sensors.

    #include <Arduino.h>
    #include <HC_SR04.h>

    //             |  Echo  |  Trigger
    // Sensor 1:   |   2    |     8
    // Sensor 2:   |   3    |     8
    // Sensor 3:   |   4    |     8
    // Sensor 4:   |   5    |    11
    HC_SR04_BASE *Slaves[] = { new HC_SR04<3>(), new HC_SR04<4>(), new HC_SR04<5>(11) };
    HC_SR04<2> sonicMaster(8, Slaves, 3);

    void setup() { Serial.begin(9600);  sonicMaster.begin(); }

    void loop() {
        sonicMaster.startMeasure(200000);
        for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++) {
            Serial.print(sonicMaster.getDist_cm(i));
            Serial.print("  ");
        }
        Serial.println();
        delay(1000);
    }


## Multi sensor example (asynchron)
Use four sensors with asynchron readout, startAsync only starts measurement. While measurement is running other tasks can be executed in main loop.
Results are available when sonicMaster.isFinished() returns true.
Time required is the time to measure the sensor with the biggest distance. (~ 4 times faster then synchron paralell measurement)
HC_SR04_ALL: execute command on all sensors. 0: master, 1..n number of slaves if only one sensor should be handled.

    #include <Arduino.h>
    #include <HC_SR04.h>

    //             |  Echo  |  Trigger
    // Sensor 1:   |   2    |     8
    // Sensor 2:   |   3    |     8
    // Sensor 3:   |   4    |     8
    // Sensor 4:   |   5    |    11
    HC_SR04_BASE *Slaves[] = { new HC_SR04<3>(), new HC_SR04<4>(), new HC_SR04<5>(11) };
    HC_SR04<2> sonicMaster(8, Slaves, 3);

    void setup() {
        Serial.begin(9600);
        sonicMaster.beginAsync();
    }

    void loop() {
        sonicMaster.startAsync(200000);
        while(!sonicMaster.isFinished()) {
            // Do something usefulle while measureing
            // all echo pins which doesnt support interrupt will have a 0 result
        }
    
        for (int i = 0; i < sonicMaster.getNumberOfSensors(); i++) {
            Serial.print(sonicMaster.getDist_cm(i));
            Serial.print("  ");
        }
        Serial.println();
        delay(1000);
    }
