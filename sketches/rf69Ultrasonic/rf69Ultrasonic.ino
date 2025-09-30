#include <UltrasonicSensor.h>
#include <Rf69Radio.h>

// sensor id
#define routNumber 9

// sensor wiring
#define triggerPin 9
#define echoPin 8

Rf69Radio radio(routNumber);

// construct sensor
UltrasonicSensor sensor(
  triggerPin,
  echoPin,
  routNumber
);

void setup() {
  radio.setup();
  // pass sensor data to coordinator through Rf69 radio
  sensor.setDataHandler(Rf69Radio::sendToCoordinator, &radio);
}

void loop() {
  radio.loop();
  sensor.loop();
}
