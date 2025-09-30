#ifndef RF69_RADIO
#define RF69_RADIO

#include <RH_RF69.h>
#include <RHReliableDatagram.h>

// configurable options
// address of coordinator
#define rf69CoordinatorAddress 0x00
// pin used by default for chip select.
// note that if an Arduino has multiple Rf69 radios, each must have a
// different chip select pin.
#define defaultRf69ChipSelectPin 4
// default frequency to use for rf69 radios
// rf69 radios should have the same frequency in order to communicate
// with each other
#define defaultRf69FrequencyMhz 915.0

// universal pins needed by rf69 radios
#define rf69InterruptPin 3
#define rf69ResetPin 2

struct Rf69Radio {
  public:
    // constructor
    Rf69Radio(
      // address (leave blank to use coordinator address)
      uint8_t = rf69CoordinatorAddress,
      // chip select pin
      uint8_t = defaultRf69ChipSelectPin,
      // frequency
      float = defaultRf69FrequencyMhz
    );

    // call during setup
    void setup();
    // call frequently
    void loop();

    // send a packet to the coordinator
    static void sendToCoordinator(const uint8_t*, size_t, void*);
    void sendToCoordinator(const uint8_t*, size_t);

    // register a function to handle incoming packets
    void setPacketHandler(void (*)(const uint8_t*, size_t));

  private:
    // all rf69 radios that communciate with each other must have the
    // same encryption key
    static const uint8_t* encryptionKey;

    // radiohead objects for controlling radio module
    RH_RF69 radioDriver;
    RHReliableDatagram radioManager;

    // frequency to use
    float frequency;

    // function for handling incoming packets.
    // while null, do not handle incoming packets
    void (*packetHandler)(const uint8_t*, size_t) = nullptr;
};

#endif
