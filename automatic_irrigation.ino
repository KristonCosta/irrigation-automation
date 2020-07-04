#include <irrigation_controller.h>
#include <client_library.h>

#include <Arduino.h>

// Pin for your first pump
int pump1 = 2;

// Pin for your first sensor
int sensor1 = A0;


// Configure the number of sensors, make sure there's a controller for each sensor
int number_of_sensors = 4;
IrrigationController *controllers[4];


void handle_irrigation();
void handle_serial();
void step_controllers();


void setup() {
    /*
     * Sets the baud rate and intializes the IrrigationControllers for each pump/sensor pair
     *
     */
    Serial.begin(9600);
    for (int i = 0; i < number_of_sensors; i++) {
        // Initialize a controller for each sensor
        controllers[i] = new IrrigationController(sensor1 + i, pump1 + i);
    }
    delay(500);
}


void loop() {
    /*
     * The main Arduino loop
     *    - run the irrigation step where we check thresholds, water plants, etc
     *    - check to see if there are any pending commands issues via the serial port
     *    - add a small delay so the loop isn't constantly firing
     *
     */
    handle_irrigation();
    handle_serial();
    delay(50);
}

void handle_irrigation() {
    /*
     * Thought this might end up doing more, but for now
     * it just steps the controllers
     *
     */
    step_controllers();
}

void step_controllers() {
    /*
     * `step` each controller
     * During a step the controller will check water thresholds and turn on/off water pumps
     *
     */
    for (auto & i : controllers) {
        i->step();
    }
}

// Handlers
/*
 * Below are all the command handlers for commands issues over the serial port.
 * To get a better understanding of what's happening here go down to handle_serial,
 * it has more information on how the commands are fetched, decoded, and issued.
 *
 * Most of the other handlers are self explanatory once you understand how commands work
 *
 */

bool validate_target(RequestBundle* bundle, byte buf[4]) {
    /*
     * Applies validation to an incoming request
     * For now it just verifies that the target pump for a command exists
     *
     * This will be called by any command that requires a target
     *
     */
    if (bundle->target > number_of_sensors - 1) {
        Encoder::encode(Response::INVALID_REQUEST, buf);
        return false;
    }
    return true;
}

void handle_ping(RequestBundle* bundle, byte buf[4]) {
    /*
     * Pong
     */
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
   /*
    * Each Serial.read() only returns 1 byte.
    *
    * We Serial.read() 4 times in order to grab all 4 bytes and store them
    * into a byte array. We can then copy the byte array into an unsigned long
    * in order to get the 32 bit integer that represents our command.
    */
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
    /*
     * Handle serial handles all Serial communication.
     *
     * 4 byte chunks are grabbed from the Serial port via the `read_command` which are converted
     * to a long.
     *
     * The long is then translated to a RequestBundle by the client_library decoder to determine
     * the command, target pump, and value associated with the command.
     *
     * We then just match the command in the bundle to a set of commands we know how to handle
     * and call the associated handler for a given command. The handlers accept a 4 byte buffer
     * which will store the commands response.
     *
     * Lastly, the 4 byte response is pushed to the Serial port and flushed.
     *
     */
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
        // clean up after yourself unless you want memory leaks
        delete bundle;
    }
}