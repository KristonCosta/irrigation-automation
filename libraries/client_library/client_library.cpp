#include "client_library.h"

RequestBundle* Decoder::decode(unsigned long command) {
    byte bytes[4];
    // Load the bytes
    load_byte_array(command, bytes);
    // Make the bundle!
    auto* bundle = new RequestBundle();
    bundle->request = Decoder::get_request(bytes);
    bundle->target = Decoder::get_target(bytes);
    bundle->value = Decoder::get_value(bytes);
    return bundle;
}

Request Decoder::get_request(const byte *command) {
    byte command_byte = command[0];
    // We can convert the command byte straight into the Request enum since they have the same "shape"
    auto r = static_cast<Request>(command_byte);
    // And then we can just use a case statement to match on them!
    switch (r) {
        case Request::PING:
        case Request::GET_MOISTURE:
        case Request::SET_THRESHOLD:
        case Request::GET_PULSE_LENGTH:
        case Request::SET_PULSE_LENGTH:
        case Request::GET_IS_RUNNING:
        case Request::GET_THRESHOLD:
        case Request::ENABLE:
        case Request::DISABLE:
        case Request::GET_FREQUENCY:
        case Request::SET_FREQUENCY:
            return r;
    }
    // If we don't match then we just return an unknown
    return Request::UNKNOWN;
}

uint8_t Decoder::get_target(const byte *command) {
    // Convenient!
    return command[1];
}

uint16_t Decoder::get_value(const byte *command) {
    // Grab the 2 bytes that represent the 16 bit value and copy them into an unsigned 16 bit int.
    uint16_t result;
    memcpy(&result, &command[2], sizeof(uint16_t));
    return result;
}

void load_byte_array(unsigned long n, byte buf[4]) {
    /*
     * Just uses bitwise AND to chop out the bits we care about
     *
     * In hindsight, I didn't really need to turn the incoming 4 bytes from the Serial port into an unsigned long, oh well.
     */
    buf[0] = n & 255u;
    buf[1] = (n >> 8u)  & 255u;
    buf[2] = (n >> 16u) & 255u;
    buf[3] = (n >> 24u) & 255u;
}

void Encoder::encode(Response response, byte buf[4]) {
    Encoder::encode(response, 0, 0, buf);
}

void Encoder::encode(Response response, uint8_t target, unsigned int value, byte *buf) {
    buf[0] = response;
    buf[1] = target;
    buf[2] = value & 0xFFu;
    buf[3] = (value >> 8u) & 0xFFu;
}