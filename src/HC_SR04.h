// HC_SR04_BASE - https://github.com/bjoernboeckle/HC_SR04
// Copyright © 2022, Björn Böckle
// MIT License

#ifndef HC_SR04_H
#define HC_SR04_H

#include <Arduino.h>

// states during async measurement
enum HCSR04_State
{
  HCSR04_IDLE,
  HCSR04_STARTED,
  HCSR04_RUNNING
};

// defines to read all or only master (default for functions only master)
#define HC_SR04_ALL    -1
#define HC_SR04_MASTER  0

class HC_SR04_BASE
{
public:
  bool begin(int slave = HC_SR04_MASTER);
  bool startMeasure(unsigned long timeout = 0, int slave = HC_SR04_MASTER);

  unsigned long getDuration_us(int slave = HC_SR04_MASTER) { return isSlaveValid(slave) ? _slaves[slave-1]->getDuration_us() : _duration_us;}
  long getDist_cm(int slave = HC_SR04_MASTER)    { return isSlaveValid(slave) ? _slaves[slave-1]->getDist_cm() : (long)(_duration_us / 58.8235); }
  long getDist_mm(int slave = HC_SR04_MASTER)    { return isSlaveValid(slave) ? _slaves[slave-1]->getDist_mm() :(long)(_duration_us / 5.88235); }
  long getDist_inch(int slave = HC_SR04_MASTER)  { return isSlaveValid(slave) ? _slaves[slave-1]->getDist_inch() : (long)(_duration_us / 148.01864); } 
  bool HasTrigger(int slave = HC_SR04_MASTER)    { return isSlaveValid(slave) ? _slaves[slave-1]->HasTrigger(): _trigger >= 0;}
  int getNumberOfSensors() {return _numSlaves + 1; }  // slaves + master sensors

  HCSR04_State getState(int slave = HC_SR04_MASTER) {return isSlaveValid(slave) ? _slaves[slave-1]->getState() : _state; }

  bool isFinished(int slave = HC_SR04_MASTER);

  bool isInterruptSupported(int slave = HC_SR04_MASTER) { return isSlaveValid(slave) ? _slaves[slave-1]->isInterruptSupported() : _interruptSupported; }
  void release(int slave = HC_SR04_MASTER);

  bool beginAsync(int slave = HC_SR04_MASTER);
  bool startAsync(unsigned long timeout = 0, int slave = HC_SR04_MASTER );

protected:
  HC_SR04_BASE() {};
  HC_SR04_BASE(int echo, int trigger = -1);
  HC_SR04_BASE(int echo, int trigger, HC_SR04_BASE* slaves[], int numSlaves);
  void handleInterrupt();

private:
  int _trigger;
  int _echo;
  int _init;
  bool _interruptSupported;
  HC_SR04_BASE **_slaves;
  int _numSlaves;
  

  unsigned long _timeout;
  volatile HCSR04_State _state;
  volatile unsigned long _start;
  volatile unsigned long _MeasurementStarted;
  volatile unsigned long _duration_us;

  void reset();
  void StartTrigger();
  bool isSlaveValid(int slave) { return  (_slaves != NULL && slave>0 && slave <=_numSlaves) ?  true : false; }

  virtual void registerInterrupt(int interruptPin){};
};


// template class to support interrupts
template <int TECHO_PIN>
class HC_SR04 : public HC_SR04_BASE
{
public:
  HC_SR04() : HC_SR04_BASE(TECHO_PIN)
  {
    _inst = this;
  }

  HC_SR04(int trigger) : HC_SR04_BASE(TECHO_PIN, trigger)
  {
    _inst = this;
  }

  HC_SR04(int trigger, HC_SR04_BASE* slaves[], int numSlaves) : HC_SR04_BASE(TECHO_PIN, trigger, slaves, numSlaves)
  {
    _inst = this;
  }


private:
  static HC_SR04<TECHO_PIN> *_inst;

  void registerInterrupt(int interruptPin)
  {
    attachInterrupt(interruptPin, HC_SR04::_echo_isr, CHANGE);
  }

  static void _echo_isr()
  {
    if (HC_SR04<TECHO_PIN>::_inst != NULL)
      HC_SR04<TECHO_PIN>::_inst->handleInterrupt();
  }
};

template <int TECHO_PIN>
HC_SR04<TECHO_PIN> *HC_SR04<TECHO_PIN>::_inst(NULL);

#endif