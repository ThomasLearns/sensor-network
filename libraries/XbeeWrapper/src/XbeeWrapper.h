#ifndef XBEE_WRAPPER
#define XBEE_WRAPPER

#include <XBee.h>
#include <AltSoftSerial.h>

#define MAX_PACKET_SIZE MAX_FRAME_DATA_SIZE

#define BROADCAST_ADDRESS 0xFF

class XbeeWrapper {
  public:
    // constructor
    XbeeWrapper(uint8_t address);

    bool available();
    bool broadcast(uint8_t*, uint8_t);
    bool sendTo(uint8_t*, uint8_t, uint8_t);
    bool receive(uint8_t*, uint8_t*, uint8_t*);

    bool setup(void (*)(String));
    void loop();

  private:
    XBee xbee;
    ZBRxResponse rx;

    // uses pins 9 (tx) and 8 (rx)
    AltSoftSerial xbeeSerial;
    uint8_t address;
};

#endif
