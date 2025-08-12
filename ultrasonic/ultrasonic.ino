#include <SensorNetworkRadio.h>

// device address on network
#define ADDRESS 1

// MHz
#define RADIO_FREQUENCY 915.0

// transceiver pins
#define CHIP_SELECT_PIN 4
#define RADIO_INTERRUPT_PIN 3
#define RADIO_RESET_PIN 2

// radio interaction object
SensorNetworkRadio radio(
  CHIP_SELECT_PIN,
  RADIO_INTERRUPT_PIN,
  RADIO_RESET_PIN,
  ADDRESS,
  RADIO_FREQUENCY
);

// runs at startup
void setup() {
  // setup serial
  Serial.begin(9600);
  while (!Serial);

  // setup radio
  radio.setup();
}

// runs on repeat
void loop() {
  // used to access the time of this loop
  unsigned long currentTimeMs = millis();

  // drive radio
  radio.loop();
}
