#ifndef _IRRIGATION_CONTROLLER_H_
#define _IRRIGATION_CONTROLLER_H_

#include "Arduino.h"
class Pulse {
    /*
     * A pulse has two parts, a frequency and a pulse length.
     *
     * The frequency is how often a pulse is fired, eg, a 5 second frequency would trigger a pulse every
     * 5 seconds.
     *
     * The pulse is how long the pump will be on for, eg, a 3 second pulse will turn a pump on for 3 seconds
     *
     * So if you have a pulse that's 3 seconds long with a frequency of 5 seconds then the pulse will fire for 3
     * seconds, then turn off for 2 seconds (the frequency counts down even while a pulse is active)
     *
     * Deactivating a pulse will immediately stop the pulse from firing.
     *
     */
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
    /*
     * The IrrigationController polls the moisture pin to see if the moisture is below a
     * threshold. If it is it activates it's internal pulse. When the internal pulse fires it turns on
     * the associated pump pin. Once the moisture level reaches acceptable levels internal pulse is deactivated.
     *
     * The step method will perform all the above steps, enabling/disabling pumps as needed.
     *
     * Refer to step() in `irrigation_controller.cpp` to get implementation details.
     */
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
