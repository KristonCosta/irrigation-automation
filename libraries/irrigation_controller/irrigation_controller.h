#ifndef _IRRIGATION_CONTROLLER_H_
#define _IRRIGATION_CONTROLLER_H_

#include "Arduino.h"
class Pulse {
public:
    explicit Pulse(unsigned int pulse_length);
    void activate();
    void deactivate();

    unsigned int getPulseLength();
    void setPulseLength(unsigned int length);

    unsigned int getFrequency();
    void setFrequency(unsigned int frequency);

    bool isFiring();
private:
    bool running;
    unsigned int frequency;
    unsigned int pulse_length;
    unsigned long last_pulse_time;
};

class IrrigationController {
public:
    IrrigationController(int moisture_pin, int pump_pin);
    unsigned int getMoisture();
    unsigned int getThreshold();
    unsigned int getPulseLength();

    void setThreshold(unsigned int threshold);
    void setPulseLength(unsigned int length);

    bool isRunning();

    void enable();
    void disable();

    unsigned int getFrequency();
    void setFrequency(unsigned int frequency);

    bool step();
private:
    unsigned int moisture_pin;
    unsigned int pump_pin;
    unsigned int humidity_threshold;
    Pulse pulse;

    bool enabled;
};



#endif //_IRRIGATION_CONTROLLER_H_
