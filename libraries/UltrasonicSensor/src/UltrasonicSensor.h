#ifndef ULTRASONIC_SENSOR
#define ULTRASONIC_SENSOR

#include <Arduino.h>  // Ultrasonic.h needs this included first
#include <Ultrasonic.h>
#include <PacketTypes.h>

// add these entries to your sketch.yaml in the libraries section
// - dir: ../libraries/XbeeRadio
// - XBee-Arduino library (0.6.0)
// - AltSoftSerial (1.4.0)

// time before sensor stops listening for echo
#define ultrasonicSensorTimeoutUs 24000UL

struct UltrasonicSensor {
  public:
    // constructor
    UltrasonicSensor(
      uint8_t,  // trigger pin
      uint8_t,  // echo pin
      uint8_t   // sensor id
    );

    // set the data callback function
    void setDataHandler(
      void (*)(const uint8_t*, size_t, void*) = nullptr,
      void* = nullptr // instance context
    );

    // call frequently
    void loop();

  private:
    // data callback
    void (*handleData)(const uint8_t*, size_t, void*);
    void* dataHandlerContext;

    // we use the Ultrasonic library
    Ultrasonic sensor;

    // sensor id
    uint8_t routNumber;
};

#endif
