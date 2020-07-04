#ifndef _CLIENT_LIBRARY_H_
#define _CLIENT_LIBRARY_H_

#include <USBAPI.h>

/*
 * Client Library
 *
 * The point of the client library is to:
 *
 *  - decode 4 byte sequences into a RequestBundle which can be then handled by the
 *  main controller.
 *  - encode ResponseBundles into 4 byte sequences that can be written out to a Serial
 *  port.
 *
 * The general structure of this 4 byte encoding is as follows:
 * 0000 0000 0000 0000 | 0000 0000 | 0000 0000
 *                                   ^ last 8 bits describe the command, eg SET_THRESHOLD_RESPONSE would be 0000 0010
 *                       ^ second set of 8 bits describes the target, eg pump 3 would be 0000 0011
 * ^ first 16 bits are used for the value (LE), eg 512 would be 0000 0000 0000 0002
 *
 *
 * So for a `SET_THRESHOLD_RESPONSE` with a target of pump `3` and a value of `512` the bytes would end up being
 * 0000 0000 0000 0002 0000 0011 0000 0010
 *
 * A bundle has 3 properties
 *   - Request/Response - this is either the incoming request or outgoing response. Refer to [Request] and [Response]
 *   enums below to see what the possible values are
 *   - target - the target pump, 0 indexed
 *   - value - a LE encoded 16 bit value that can be consumed when handling a command. Eg, the threshold value in a
 *   SET_THRESHOLD request
 *
 *
 * The actual implementation details are pretty straightforward, refer to `client_library.cpp` for full implementation.
 */

typedef enum {
    PING                = 0x00,
    GET_MOISTURE        = 0x01,
    SET_THRESHOLD       = 0x02,
    GET_PULSE_LENGTH    = 0x03,
    SET_PULSE_LENGTH    = 0x04,
    GET_IS_RUNNING      = 0x05,
    GET_THRESHOLD       = 0x06,
    ENABLE              = 0x07,
    DISABLE             = 0x08,
    GET_FREQUENCY       = 0x09,
    SET_FREQUENCY       = 0x0A,
    UNKNOWN             = 0xFF,
} Request;

struct RequestBundle {
    Request request;
    uint8_t target;
    uint16_t value;
};

typedef enum {
    PONG                       = 0x00,
    MOISTURE_RESPONSE          = 0x01,
    SET_THRESHOLD_RESPONSE     = 0x02,
    PULSE_LENGTH_RESPONSE      = 0x03,
    SET_PULSE_LENGTH_RESPONSE  = 0x04,
    IS_RUNNING                 = 0x05,
    THRESHOLD_RESPONSE         = 0x06,
    ENABLE_RESPONSE            = 0x07,
    DISABLE_RESPONSE           = 0x08,
    FREQUENCY_RESPONSE         = 0x09,
    SET_FREQUENCY_RESPONSE     = 0x0A,
    INVALID_REQUEST            = 0xFF,
} Response;

void load_byte_array(unsigned long n, byte buf[4]);

class Decoder {
public:
    static RequestBundle* decode(unsigned long command);
private:
    static Request get_request(const byte* command);
    static uint8_t get_target(const byte* command);
    static uint16_t get_value(const byte* command);
};

class Encoder {
public:
    static void encode(Response response, byte buf[4]);
    static void encode(Response response, uint8_t target, unsigned int value, byte buf[4]);
};

#endif //_CLIENT_LIBRARY_H_
