#include <irrigation_controller.h>
#include <client_library.h>

#include <Arduino.h>

int pump1 = 2;
int sensor1 = A0;

IrrigationController *controllers[4];

int number_of_sensors = 1;

void handle_irrigation();
void handle_serial();
void step_controllers();


void setup() {
    Serial.begin(9600);
    for (int i = 0; i < 4; i++) {
        controllers[i] = new IrrigationController(sensor1 + i, pump1 + i);
    }

    delay(500);
}


void loop() {
    handle_irrigation();
    handle_serial();
    delay(50);
}

void handle_irrigation() {
    step_controllers();
}

void step_controllers() {
    controllers[0]->step();

    /*
    for (auto & i : controllers) {
        i->step();
    }
     */
}

// Handlers

bool validate_target(RequestBundle* bundle, byte buf[4]) {
    if (bundle->target > number_of_sensors - 1) {
        Encoder::encode(Response::INVALID_REQUEST, buf);
        return false;
    }
    return true;
}

void handle_ping(RequestBundle* bundle, byte buf[4]) {
    Encoder::encode(Response::PONG, buf);
}

void handle_get_moisture(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        unsigned int moisture = controllers[bundle->target]->getMoisture();
        Encoder::encode(Response::MOISTURE_RESPONSE, bundle->target, moisture, buf);
    }
}

void handle_set_threshold(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        controllers[bundle->target]->setThreshold(bundle->value);
        Encoder::encode(Response::SET_THRESHOLD_RESPONSE, bundle->target, 0, buf);
    }
}

void handle_get_pulse_length(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        unsigned int length = controllers[bundle->target]->getPulseLength();
        Encoder::encode(Response::PULSE_LENGTH_RESPONSE, bundle->target, length, buf);
    }
}

void handle_set_pulse_length(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        controllers[bundle->target]->setPulseLength(bundle->value);
        Encoder::encode(Response::SET_PULSE_LENGTH_RESPONSE, bundle->target, 0, buf);
    }
}

void handle_get_is_running(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        bool isRunning = controllers[bundle->target]->isRunning();
        Encoder::encode(Response::IS_RUNNING, bundle->target, isRunning, buf);
    }
}

void handle_get_threshold(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        unsigned int threshold = controllers[bundle->target]->getThreshold();
        Encoder::encode(Response::THRESHOLD_RESPONSE, bundle->target, threshold, buf);
    }
}

void handle_enable(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        controllers[bundle->target]->enable();
        Encoder::encode(Response::ENABLE_RESPONSE, bundle->target, 1, buf);
    }
}

void handle_disable(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        controllers[bundle->target]->disable();
        Encoder::encode(Response::DISABLE_RESPONSE, bundle->target, 1, buf);
    }
}

void handle_get_frequency(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        unsigned int threshold = controllers[bundle->target]->getFrequency();
        Encoder::encode(Response::FREQUENCY_RESPONSE, bundle->target, threshold, buf);
    }
}

void handle_set_frequency(RequestBundle* bundle, byte buf[4]) {
    if (validate_target(bundle, buf)) {
        controllers[bundle->target]->setFrequency(bundle->value);
        Encoder::encode(Response::SET_FREQUENCY_RESPONSE, bundle->target, 1, buf);
    }
}

unsigned long read_command() {
    unsigned char bytes[4] {
            (byte) Serial.read(),
            (byte) Serial.read(),
            (byte) Serial.read(),
            (byte) Serial.read()
    };
    unsigned long result;
    memcpy(&result, bytes, sizeof(unsigned long));
    return result;
}


void handle_serial() {
    if (Serial.available() > 0) {
        unsigned long incoming = read_command();
        RequestBundle* bundle = Decoder::decode(incoming);
        byte buf[4] = {0, 0, 0, 0};
        switch (bundle->request) {
            case Request::PING:
                handle_ping(bundle, buf);
                break;
            case Request::GET_MOISTURE:
                handle_get_moisture(bundle, buf);
                break;
            case Request::SET_THRESHOLD:
                handle_set_threshold(bundle, buf);
                break;
            case Request::GET_PULSE_LENGTH:
                handle_get_pulse_length(bundle, buf);
                break;
            case Request::SET_PULSE_LENGTH:
                handle_set_pulse_length(bundle, buf);
                break;
            case Request::GET_IS_RUNNING:
                handle_get_is_running(bundle, buf);
                break;
            case Request::GET_THRESHOLD:
                handle_get_threshold(bundle, buf);
                break;
            case Request::ENABLE:
                handle_enable(bundle, buf);
                break;
            case Request::DISABLE:
                handle_disable(bundle, buf);
                break;
            case Request::GET_FREQUENCY:
                handle_get_frequency(bundle, buf);
                break;
            case Request::SET_FREQUENCY:
                handle_set_frequency(bundle, buf);
                break;
            case Request::UNKNOWN:
                Encoder::encode(Response::INVALID_REQUEST,  buf);
        }
        Serial.write(buf, sizeof(buf));
        Serial.flush();
        delete bundle;
    }
}


/* Request
 * 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * Possible requests
 * PING -> keep alive(zzzz is request tag)
 * 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * GET_MOISTURE (xxxx is pump number, zzzz is request tag)
 * 0000 0001 0000 xxxx 0000 0000 0000 0000
 *
 * SET_THRESHOLD (xxxx is pump number, yyyy is u16, zzzz is request tag)
 * 0000 0010 0000 xxxx yyyy yyyy yyyy yyyy
 *
 * GET_PULSE_LENGTH (xxxx is pump number, zzzz is request tag)
 * 0000 0011 0000 xxxx 0000 0000 0000 0000
 *
 * SET_PULSE_LENGTH (xxxx is pump number, yyyy is u8, zzzz is request tag)
 * 0000 0100 0000 xxxx 0000 0000 yyyy yyyy
 *
 * GET_IS_RUNNING (xxxx is pump number, zzzz is request tag)
 * 0000 0101 0000 xxxx 0000 0000 0000 0000
 *
 * GET_THRESHOLD (xxxx is pump number, yyyy is u16, zzzz is request tag)
 * 0000 0110 0000 xxxx 0000 0000 0000 0000
 *
 * ENABLE (xxxx is pump number)
 * 0000 0111 0000 xxxx 0000 0000 0000 0000
 *
 * DISABLE (xxxx is pump number)
 * 0000 0111 0000 xxxx 0000 0000 0000 0000
 *
 * Response
 * 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * Possible response
 * PONG -> keep alive
 * 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * MOISTURE_RESPONSE (xxxx is pump number, yyyy is u16)
 * 0000 0001 0000 xxxx yyyy yyyy yyyy yyyy
 *
 * SET_THRESHOLD_RESPONSE (xxxx is pump number, b is boolean)
 * 0000 0010 0000 xxxx 0000 0000 0000 000b
 *
 * PULSE_LENGTH_RESPONSE (xxxx is pump number)
 * 0000 0011 0000 xxxx yyyy yyyy yyyy yyyy
 *
 * SET_PULSE_LENGTH_RESPONSE (xxxx is pump number, b is boolean)
 * 0000 0100 0000 xxxx 0000 0000 0000 000b
 *
 * GET_IS_RUNNING (xxxx is pump number, b is boolean)
 * 0000 0101 0000 xxxx 0000 0000 0000 000b
 *
 * INVALID_REQUEST
 * 1111 1111 0000 xxxx 0000 0000 0000 0000
 */
