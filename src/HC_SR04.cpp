// HC_SR04_BASE - https://github.com/bjoernboeckle/HC_SR04_BASE
// Copyright © 2022, Björn Böckle
// MIT License

#include "HC_SR04.h"


HC_SR04_BASE::HC_SR04_BASE(int echo, int trigger)
{
    _slaves = NULL;
    _numSlaves = 0;
    _trigger = trigger;
    _echo = echo;
    _init = false;
    _interruptSupported = false;
}

HC_SR04_BASE::HC_SR04_BASE(int echo, int trigger, HC_SR04_BASE *slaves[], int numSlaves)
{
    _trigger = trigger;
    _echo = echo;
    _init = false;
    _interruptSupported = false;    

    _slaves = slaves;
    _numSlaves = numSlaves;
}

bool HC_SR04_BASE::begin(int slave)
{
    if(isSlaveValid(slave))
        return _slaves[slave-1]->begin();
    else if ( slave == HC_SR04_ALL)
    {
        bool bResult = begin();
        for (int i=0; i<_numSlaves; i++)
            bResult &= _slaves[i]->begin();
        return bResult;
    }

    reset();
    pinMode(_echo, INPUT);
    if (HasTrigger()) {
        pinMode(_trigger, OUTPUT);
        digitalWrite(_trigger, LOW);
    }
    _init = true;

    return true;
}


void HC_SR04_BASE::StartTrigger() 
{
    if (!HasTrigger())
        return;

    digitalWrite(_trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigger, LOW);
}


bool HC_SR04_BASE::startMeasure(unsigned long timeout, int slave)
{
    if(isSlaveValid(slave))
    {
        // Measure only a slave sensor, in case slave doesn't has an own trigger use the master trigger
        if (!_slaves[slave-1]->HasTrigger())
            StartTrigger();        
        return _slaves[slave-1]->startMeasure(HC_SR04_MASTER, timeout);
    }
    if (slave == HC_SR04_ALL)
    {
        // Measure all sensors, in case slave doesn't has an own trigger use the master trigger
        bool bResult = startMeasure(timeout);
        for (int i=0; i<_numSlaves; i++)
        {
            delay(1);  // small delay between subsequent measurements
            if (!_slaves[i]->HasTrigger())
                StartTrigger();
            bResult &= _slaves[i]->startMeasure(HC_SR04_MASTER, timeout);            
        }
        return bResult;
    }    

    // perform master measurement
    StartTrigger();

    if (timeout == 0)
        _duration_us = pulseIn(_echo, HIGH);
    else
        _duration_us = pulseIn(_echo, HIGH, timeout);

    return _duration_us > 0 ? true : false;
}



void HC_SR04_BASE::release(int slave)
{
    if(isSlaveValid(slave))
        return _slaves[slave-1]->release();

    if (slave == HC_SR04_ALL)
    {
        release();
        for (int i=0; i<_numSlaves; i++)
            _slaves[i]->release();
        return;
    }

    reset();
    if (HasTrigger())
        pinMode(_trigger, INPUT);
    pinMode(_echo, INPUT);
    _init = false;

    if ( isInterruptSupported())
    {
        int interruptPin = digitalPinToInterrupt(_echo);
        if (interruptPin != NOT_AN_INTERRUPT)
            detachInterrupt(interruptPin);
    }
    _interruptSupported = false;
}

void HC_SR04_BASE::reset()
{
    _duration_us = 0;
    _start = 0;
    _MeasurementStarted = 0;
    _timeout = 0;
    _state = HCSR04_IDLE;
}

bool HC_SR04_BASE::beginAsync(int slave)
{
    if(isSlaveValid(slave))
        return _slaves[slave-1]->beginAsync();

    if (slave == HC_SR04_ALL)
    {
        // beginn all sensors
        bool bResult = beginAsync();
        for (int i=0; i<_numSlaves; i++)
            bResult &= _slaves[i]->beginAsync();
        return bResult;
    }

    begin();

    int interruptPin = digitalPinToInterrupt(_echo);
    if (interruptPin == NOT_AN_INTERRUPT)
        return false;
    registerInterrupt(interruptPin);
    _interruptSupported = true;
    
    return true;
}

bool HC_SR04_BASE::startAsync(unsigned long timeout, int slave)
{
    if(isSlaveValid(slave))
        return _slaves[slave-1]->startAsync(HC_SR04_MASTER, timeout);

    if (slave == HC_SR04_ALL)
    {
        // beginn all sensors
        bool bResult = true;
        for (int i=0; i<_numSlaves; i++)
            bResult &= _slaves[i]->startAsync(HC_SR04_MASTER, timeout);
        // last master to trigger after all slaves are started
        bResult &= startAsync(HC_SR04_MASTER, timeout);
        return bResult;
    }

    reset();
    _timeout = timeout;
    _MeasurementStarted = micros();

    if (!_interruptSupported)
        return false;

    _state = HCSR04_STARTED;
    StartTrigger();

    return true;
}

bool HC_SR04_BASE::isFinished(int slave)
{
    if(isSlaveValid(slave))
        return _slaves[slave-1]->isFinished();
    if (slave == HC_SR04_ALL)
    {
        bool bFinished = isFinished();
        for (int i=0; i<_numSlaves; i++)
            bFinished &= _slaves[i]->isFinished();
        return bFinished;
    }

    if (_timeout > 0 && _state != HCSR04_IDLE && micros() - _MeasurementStarted > _timeout)
    {
        _state = HCSR04_IDLE;
        _duration_us = 0;
    }
    return _state == HCSR04_IDLE;
}

void HC_SR04_BASE::handleInterrupt()
{
    switch (digitalRead(_echo))
    {
    case HIGH:
        if (_state == HCSR04_STARTED)
        {
            _start = micros();
            _state = HCSR04_RUNNING;
        }
        break;
    case LOW:
        if (_state == HCSR04_RUNNING)
        {
            _duration_us = micros() - _start;
            _state = HCSR04_IDLE;
        }
        break;
    }
}