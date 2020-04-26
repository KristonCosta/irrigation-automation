#ifndef _CLIENT_LIBRARY_H_
#define _CLIENT_LIBRARY_H_

#include <USBAPI.h>

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
