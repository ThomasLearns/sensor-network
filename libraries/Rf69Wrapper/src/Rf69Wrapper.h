#ifndef RF69_WRAPPER
#define RF69_WRAPPER

#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define MAX_PACKET_SIZE RH_RF69_MAX_MESSAGE_LEN
#define INTERRUPT_PIN 3
#define RESET_PIN 2

namespace Rf69Constants {
  // defined here so it can be the same on all devices in our network
  extern uint8_t encryptionKey[16];
}

class Rf69Wrapper {
  public:

    Rf69Wrapper(uint8_t, uint8_t, float);

    bool available();
    bool broadcast(uint8_t*, uint8_t);
    bool sendTo(uint8_t*, uint8_t, uint8_t);
    bool receive(uint8_t*, uint8_t*, uint8_t*);

    bool setup(void (*)(String));
    void loop();

  private:

    RH_RF69 radioDriver;
    RHReliableDatagram radioManager;
    float frequency;
};

#endif
