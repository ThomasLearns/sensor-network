#include <SensorNetworkRadio.h>

// device address
// should be different for each device on network
#define ADDRESS 0

// transeiver pins
#define CHIP_SELECT_PIN 4
#define RADIO_INTERRUPT_PIN 3
#define RADIO_RESET_PIN 2

// MHz
#define RADIO_FREQUENCY 915.0

// create the radio interaction object
SensorNetworkRadio radio(
  CHIP_SELECT_PIN,
  RADIO_INTERRUPT_PIN,
  RADIO_RESET_PIN,
  ADDRESS,
  RADIO_FREQUENCY
);

// send out discovery packets at a fixed interval
unsigned long discoveryIntervalMs = 1000;
unsigned long lastDiscoveryMs = 0;

// baud rate for serial communication (communication with GUI)
#define SERIAL_BAUD_RATE 115200

// send the data to the GUI
void handleDataPacket(uint8_t* packet, uint8_t packetLength) {
  Serial.write(0x05); // marker for data packet
  Serial.write(packet, packetLength);
  Serial.print("\n"); // GUI reads \n as end of packet
}

// runs once at startup
void setup() {
  // setup serial
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) ;

  // setup radio
  radio.setup();
  radio.dataHandler = handleDataPacket;
}

// runs on repeat
void loop() {
  // get the current time to measure timing of some tasks
  unsigned long currentTimeMs = millis();

  // send discovery packets on an interval
  if (currentTimeMs - lastDiscoveryMs >= discoveryIntervalMs) {
    radio.sendDiscoveryPacket();
    lastDiscoveryMs = currentTimeMs;
  }

  // drive the radio
  radio.loop();
}
