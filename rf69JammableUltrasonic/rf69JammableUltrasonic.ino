#include <UltrasonicSensor.h>
#include <Rf69Radio.h>
#include <PhotodiodeJammer.h>

// Used for an ultrasonic sensor device with an attached photodiode jammer that
// uses an RF69 transceiver to communicate with the coordinator.

// sensor id
#define routNumber 9

// sensor wiring
#define triggerPin 9
#define echoPin 8

#define jammerPin A1
#define jamFrequency laserMedium

Rf69Radio radio(routNumber);

// construct sensor
UltrasonicSensor sensor(
  triggerPin,
  echoPin,
  routNumber
);

PhotodiodeJammer jammer(jammerPin, jamFrequency);

void setup() {
  radio.setup();
  jammer.setup();
  
  // give jammer opportunity to block data from sensor
  sensor.setDataHandler(PhotodiodeJammer::pipeJammablePacket, &jammer);
  
  // pass unjammed data packets to coordinator through rf69 radio
  jammer.setUnjammedPacketHandler(Rf69Radio::sendToCoordinator, &radio);
}

void loop() {
  radio.loop();
  sensor.loop();
  jammer.loop();
}
