#ifndef XBEE_WRAPPER
#define XBEE_WRAPPER

#include <XBee.h>
#include <AltSoftSerial.h>

#define MAX_PACKET_SIZE MAX_FRAME_DATA_SIZE

class XbeeWrapper {
  public:
    // constructor
    XbeeWrapper(bool useSoftwareSerial = false);

    bool available();
    bool broadcast(uint8_t*, uint8_t);
    bool sendToCoordinator(uint8_t*, uint8_t);
    bool receive(uint8_t*, uint8_t*);

    bool setup(void (*)(String));
    void loop();

  private:
    XBee xbee;
    Rx16Response rx;

    // uses pins 9 (tx) and 8 (rx) if in use. otherwise pins 1 (tx) and 0 (rx) are used
    AltSoftSerial xbeeSerial;
    bool useSoftwareSerial;
};

#endif
