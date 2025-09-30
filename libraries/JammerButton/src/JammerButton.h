#ifndef JAMMER_BUTTON
#define JAMMER_BUTTON

#include <Arduino.h>
#include <PacketTypes.h>

struct JammerButton {
  public:
    // constructor
    JammerButton(
      uint8_t,  // pin connected to jam button
      uint8_t   // sensor id
    );

    void setJamHandler(void (*)(const uint8_t*, size_t, void*), void* = nullptr);

    // call from setup function
    void setup();
    // call frequently
    void loop();

  private:
    uint8_t buttonPin;
    uint8_t routNumber;

    void (*packetHandler)(const uint8_t*, size_t, void*) = nullptr;
    void* packetHandlerContext = nullptr;
};

#endif
