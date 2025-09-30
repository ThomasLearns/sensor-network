#include <XbeeRadio.h>
#include <UltrasonicSensor.h>

// Collect data from an ultrasonic sensor and send it to the coordinator
// with an XBee radio module.

// sensor id
#define routNumber 9

// sensor wiring
#define triggerPin 12
#define echoPin 11

// construct sensor
UltrasonicSensor sensor(
  triggerPin,
  echoPin,
  routNumber
);

void setup() {
  xbee::setup();
  sensor.setDataHandler(xbee::sendToCoordinator);
}

void loop() {
  xbee::loop();
  sensor.loop();
}
