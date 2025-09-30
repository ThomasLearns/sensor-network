#include "Rf69Radio.h"

// define encryption key for all rf69 radios
static const uint8_t rf69EncryptionKey[] = {
  0x29, 0x2f, 0x07, 0x6b, 0x91, 0xf2, 0xa4, 0x02,
  0x60, 0xb5, 0x07, 0x88, 0x0f, 0x1e, 0x61, 0x72
};
const uint8_t* Rf69Radio::encryptionKey = rf69EncryptionKey;

// constructor
Rf69Radio::Rf69Radio(
  uint8_t address = rf69CoordinatorAddress,
  uint8_t chipSelectPin = defaultRf69ChipSelectPin,
  float frequency = defaultRf69FrequencyMhz
):
  radioDriver(chipSelectPin, rf69InterruptPin),
  radioManager(radioDriver, address),
  frequency(frequency)
{}

void Rf69Radio::setup() {
  // reset the transceiver
  pinMode(rf69ResetPin, OUTPUT);
  digitalWrite(rf69ResetPin, LOW);
  delay(10);
  digitalWrite(rf69ResetPin, HIGH);
  delay(10);
  digitalWrite(rf69ResetPin, LOW);
  delay(10);

  // initialize the radio manager
  radioManager.init();

  // set the radio's frequency
  radioDriver.setFrequency(frequency);

  // configure the radio
  radioDriver.setTxPower(20, true);
  radioDriver.setEncryptionKey(encryptionKey);
}

void Rf69Radio::loop() {
  // handle all incoming packets
  while (radioManager.available()) {
    // load the packet into memory
    uint8_t packet[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t packetSize;
    uint8_t sourceAddress;
    radioManager.recvfrom(packet, &packetSize, &sourceAddress);

    // handle the incoming packet if we have a packet handler function
    if (packetHandler == nullptr) continue;
    packetHandler(packet, packetSize);
  }
}

// send a packet to the coordinator
static void Rf69Radio::sendToCoordinator(
  const uint8_t* packet,
  size_t packetSize,
  void* rf69RadioContext
) {
  static_cast<Rf69Radio*>(rf69RadioContext)->sendToCoordinator(packet, packetSize);
}
void Rf69Radio::sendToCoordinator(const uint8_t* packet, size_t packetSize) {
  radioManager.sendto(packet, packetSize, rf69CoordinatorAddress);
}

// set the function that handles incoming packets
void Rf69Radio::setPacketHandler(void (*newPacketHandler)(const uint8_t*, size_t)) {
  packetHandler = newPacketHandler;
}
