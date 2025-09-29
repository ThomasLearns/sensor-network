#ifndef XBEE_RADIO
#define XBEE_RADIO

#include <XBee.h>
#include <AltSoftSerial.h>

// place the following entries in the libraries section of your sketch.yaml
// - dir: ../libraries/XbeeRadio
// - XBee-Arduino library (0.6.0)
// - AltSoftSerial (1.4.0)

namespace xbee {
    // a different baud rate is used for the xbee module depending on the type
    // of serial used to communicate. Ensure that the xbee module's configuration has
    // the same baud rate
    #define xbeeSoftwareSerialBaudRate 57600
    #define xbeeHardwareSerialBaudRate 115200

    // sets up xbee.
    // pass in a callback that is called whenever a packet is received
    // use the 2nd argument when you want to use software serial (when hardware serial is
    // needed by another component, such as the GUI). Note that if using software serial,
    // pins 8 and 9 must be unused by all other components. Additionally, you must connect
    // pins 8 and 9 to pins 2 and 3 respectively.
    // !!IMPORTANT!!
    // If an arduino is set to not use software serial here, you should ensure that
    // the switch on the top board is set to DLINE when uploading code, and UART when
    // in use. If not set to DLINE when uploading, the upload will likely fail.
    void setup(void (*finalPacketHandler)(const uint8_t*, size_t) = nullptr, bool useSoftwareSerial = false);
    
    // run every loop
    void loop();

    // send a message to the coordinator xbee module
    void sendToCoordinator(uint8_t*, size_t);
};

#endif
