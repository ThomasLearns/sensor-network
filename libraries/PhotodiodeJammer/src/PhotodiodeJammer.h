#ifndef PHOTODIODE_JAMMER_H
#define PHOTODIODE_JAMMER_H

#include <Arduino.h>

// measured in Hz
#define laserLow 900.0
#define laserMedium 1000.0
#define laserHigh 1100.0

#define jamCheckTimeoutUs 5000UL
#define jamFrequencyLeewayHz 10.0
#define maxJamDurationMs 200UL

struct PhotodiodeJammer {
  public:
    PhotodiodeJammer(uint8_t, float);

    void setup();
    void loop();

    static void pipeJammablePacket(const uint8_t*, size_t, void*);
    void pipeJammablePacket(const uint8_t*, size_t);

    void setUnjammedPacketHandler(
      void (*)(const uint8_t*, size_t, void*),
      void* = nullptr
    );

  private:
    void (*packetHandler)(const uint8_t*, size_t, void*) = nullptr;
    void* packetHandlerContext = nullptr;

    float targetFrequencyHz;

    uint8_t photodiodePin;

    unsigned long jamDurationMs = 0;

    bool isJammed();
    unsigned long lastLoopTimestampMs = 0;
};

#endif
