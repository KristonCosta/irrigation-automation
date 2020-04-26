#include "irrigation_controller.h"

IrrigationController::IrrigationController(int moisture_pin, int pump_pin)
: pulse(Pulse(2000)) {
    this->moisture_pin = moisture_pin;
    this->pump_pin = pump_pin;
    this->humidity_threshold = 1000;
    this->enabled = true;

    pinMode(moisture_pin, INPUT);
    pinMode(pump_pin, OUTPUT);
    digitalWrite(this->pump_pin, HIGH);
}

unsigned int IrrigationController::getPulseLength() {
    return this->pulse.getPulseLength();
}

unsigned int IrrigationController::getMoisture() {
    return analogRead(this->moisture_pin);
}

unsigned int IrrigationController::getThreshold() {
    return this->humidity_threshold;
}

void IrrigationController::setThreshold(unsigned int threshold) {
    this->humidity_threshold = threshold;
}

void IrrigationController::setPulseLength(unsigned int length) {
    this->pulse.setPulseLength(length);
}

bool IrrigationController::step() {
    if (!this->enabled) {
        return false;
    }
    unsigned int humidity = this->getMoisture();
    if (humidity > this->humidity_threshold) {
        pulse.activate();
    } else {
        pulse.deactivate();
    }
    bool is_on = pulse.isFiring();
    if (is_on) {
        digitalWrite(this->pump_pin, LOW);
    } else {
        digitalWrite(this->pump_pin, HIGH);
    }
    return is_on;
}

bool IrrigationController::isRunning() {
    return this->pulse.isFiring();
}

void IrrigationController::enable() {
    this->enabled = true;
}

void IrrigationController::disable() {
    this->enabled = false;
    digitalWrite(this->pump_pin, HIGH);
    this->pulse.deactivate();
}

void IrrigationController::setFrequency(unsigned int frequency) {
    this->pulse.setFrequency(frequency);
}

unsigned int IrrigationController::getFrequency() {
    return this->pulse.getFrequency();
}

Pulse::Pulse(unsigned int pulse_length) {
    this->pulse_length = pulse_length;
    this->last_pulse_time = millis();
    this->frequency = 5000;
    this->running = false;
}

void Pulse::activate() {
    this->running = true;
}

void Pulse::deactivate() {
    this->running = false;
}

unsigned int Pulse::getPulseLength() {
    return this->pulse_length;
}

void Pulse::setPulseLength(unsigned int length) {
    this->pulse_length = length;
}

bool Pulse::isFiring() {
    unsigned long current_time = millis();
    if (this->running && (current_time - this->last_pulse_time > this->frequency)) {
        this->last_pulse_time = current_time;
    }

    return this->running && (current_time - this->last_pulse_time < this->pulse_length);
}

unsigned int Pulse::getFrequency() {
    return this->frequency;
}

void Pulse::setFrequency(unsigned int frequency) {
    this->frequency = frequency;
}