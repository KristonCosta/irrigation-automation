## Irrigation Automation

### What is this? 
A simple controller that allows you to control water pumps via 
an Ardunio. 

### What's required? 

- An Arduino
- Moisture Sensors
- Water Pumps 


### How do you build it? 

- Set up the Ardunio IDE (https://www.arduino.cc/en/main/software) and make sure
it can connect with your board 
- Install the contents `libraries` folder of this repo into the Ardunio libraries 
folder (https://www.arduino.cc/en/guide/libraries manual installation step). 
    - If you're on Mac the final result might look something like 
    ```
  /Users/<username>/Documents/Arduino/libraries/client_library
    ```
    - If you want to develop straight from this repo then you can symlink 
    the folders 
    ```
    ln -s  /path/to/automatic_irrigation/libraries/client_library /Users/<username>/Documents/Arduino/libraries/client_library
    ```
- Load up `automatic_irrigation.ino` in the Ardunio IDE.
- Verify it builds
- Upload it!

### How does it work?
There are three main parts to this repo. The main `automatic_irrigation.ino` file and the 
two libraries `client_library` and `irrigation_controller`

#### automatic_irrigation.ino
This is primarily used for setting up an `irrigation_controller` for each moisture sensor/pump pair 
and for polling for incoming Serial commands. The file is heavily commented and can provide additional information.

#### libraries/irrigation_controller
The irrigation controller operates on a single sensor/pump pair. It's job is to poll the sensor and determine
if the moisture levels are below a given threshold. Once the moisture drops below a threshold it will send a 
pulse of water periodically until the moisture is back to a reasonable level. 

The `PulseLength` is the length of time in seconds that the pump will be active. The `Frequency` defines
how many seconds the controller will wait before issuing another pulse. Eg, if you have a `PulseLength` of 3 seconds
and a `Frequency` of 5 seconds then the controller will turn the water motor on for 3 seconds, then turn it off for 
2 seconds before repeating. 

More information can be found in `libraries/irrigation_controller.h` and `libraries/irrigation_controller.cpp`. 

#### libraries/client_library
This library is strictly for encoding and decoding a set of 4 bytes into a command that can then be consumed by the 
top level `automatic_irrigation.ino` command handler. `automatic_irrigation.ino` grabs 4 bytes from the serial port 
and sends it to the `client_library` to be decoded. The `client_library` converts those 4 bytes into:
- a request - refer to [Commands] below for more info on possible requests
- a target - optionally refers to an `irrigaton_controller` to issue the command to
- a value - optionally provides a value that will be used by the command. 

Eg, if you want to set the `threshold` for your first pump to `500` then the request might end up being: 
- request - `setThreshold`
- target - `0` (the first pump)
- value - `500`

The client library can then encode a response back into 4 bytes which can then be used as a response.

More information can be found in `libraries/client_library.h` and `libraries/client_library.h`.

### How do you configure it?
`automatic_irrigation` has a few settings you can configure. `pump1` and `sensor` define the pins your first pump/sensor 
pair are connected to. The assumption is that all your pumps are connected sequentially, eg, pump 1 connects to pin 2, 
pump 2 connects to pin 3, etc. `number_of_sensors` is the number of pump/sensor pairs that you have in total.

Specific pump and threshold settings can be configured at runtime. You can issue a series of commands via the Arduinos serial
port in order to configure each pump. Refer to [Commands] below to see which commands are available and [Format] to understand
how to format your command correctly.


#### [Commands] What are the possible commands? 
Right now the supported commands are: 
- `PING` [0x00] - pings the Arduino, sends back a `PONG` [0x00] response 
- `GET_MOISTURE` [0x01] - gets the moisture value for a target pump, you'll get a `MOISTURE_REPSONSE` [0x01] back. 
- `SET_THRESHOLD_RESPONSE` [0x02] - sets the threshold for a target pump, you'll get a `SET_THRESHOLD_RESPONSE` [0x02] back. 
- `PULSE_LENGTH_RESPONSE` [0x03] - gets the pulse length value for a target pump, you'll get a `PULSE_LENGTH_RESPONSE` [0x03] back. 
- `SET_PULSE_LENGTH_RESPONSE` [0x04] - sets the pulse length for a target pump, you'll get a `SET_PULSE_LENGTH_RESPONSE` [0x04] back. 
- `IS_RUNNING` [0x05] - checks if the target pump is running, you'll get a `IS_RUNNING` [0x05] back.
- `THRESHOLD_RESPONSE` [0x06] - gets the threshold for a target pump, you'll get a `THRESHOLD_RESPONSE` [0x06] back.
- `ENABLE_RESPONSE` [0x07] - enables a target pump, you'll get a `ENABLE_RESPONSE` [0x07] back.
- `DISABLE_RESPONSE` [0x08] - disables a target pump, you'll get a `DISABLE_RESPONSE` [0x08] back.
- `FREQUENCY_RESPONSE` [0x09] - gets the frequency value for a target pump, you'll get a `FREQUENCY_RESPONSE` [0x09] back.
- `SET_FREQUENCY_RESPONSE` [0x0A] - sets the frequency value for a target pump, you'll get a `SET_FREQUENCY_RESPONSE` [0x0A] back.
- `INVALID_REQUEST` [0xFF] - you probably don't want to send this, but if your command isn't understood you'll get a `INVALID_REQUEST` [0xFF] back. 

#### [Format] What's the structure for the 4 byte command?
```
 * 0000 0000 0000 0000 | 0000 0000 | 0000 0000
 *                                   ^ last 8 bits describe the command, eg SET_THRESHOLD_RESPONSE would be 0000 0010
 *                       ^ second set of 8 bits describes the target, eg pump 3 would be 0000 0011
 * ^ first 16 bits are used for the value (LE), eg 512 would be 0000 0000 0000 0002 
```

So for a `SET_THRESHOLD_RESPONSE` with a target of pump `3` and a value of `512` the bytes would end up being 
```
* 0000 0000 0000 0002 0000 0011 0000 0010
```

#### Final Note
I don't claim this is an great solution, but it works! If you have any suggestions/ideas feel free to open an issue
or submit a PR. 
