#ifndef SENSOR_NETWORK_RADIO
#define SENSOR_NETWORK_RADIO

#include <RH_RF69.h>
#include <RHReliableDatagram.h>

// wrapper for RadioHead radio so that we can handle Game of Drones packet formats
class SensorNetworkRadio {
  private:
    // Radiohead objects
    RH_RF69 radioDriver;
    RHReliableDatagram radioManager;

    // radio parameters
    static uint8_t encryptionKey[16];
    float frequency;  // MHz
    uint8_t resetPin;

  public:
    // constructor
    SensorNetworkRadio(
      uint8_t chipSelectPin,
      uint8_t interruptPin,
      uint8_t resetPin,
      uint8_t address,
      float frequency
    );

    // driving functions
    void setup();
    void loop();

    // event handlers to be set
    void (*dataHandler)(uint8_t[], uint8_t) = nullptr;
    void (*registrationHandler)(uint8_t) = nullptr;

    // functions for sending packets
    void sendDiscoveryPacket();
    void sendDataPacket(uint8_t*, uint8_t, uint8_t);
};

#endif
