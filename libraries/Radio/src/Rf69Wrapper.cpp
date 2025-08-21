#include "Rf69Wrapper.h"

// encryption key should be the same for all radios we use
uint8_t Rf69Constants::encryptionKey[16] = {
  0x29, 0x2f, 0x07, 0x6b, 0x91, 0xf2, 0xa4, 0x02,
  0x60, 0xb5, 0x07, 0x88, 0x0f, 0x1e, 0x61, 0x72
};

// constructor
// save the frequency and create the radio's driver and manager objects
RadioWrapper::RadioWrapper(uint8_t address, uint8_t chipSelectPin, float frequency):
  radioDriver(chipSelectPin, INTERRUPT_PIN),
  radioManager(radioDriver, address),
  frequency(frequency)
{}

// determine if a packet is available from receive()
bool RadioWrapper::available() {
  return radioManager.available();
}

// send a message to all other devices on the network
bool RadioWrapper::broadcast(uint8_t* packet, uint8_t packetLength) {
  return sendTo(packet, packetLength, RH_BROADCAST_ADDRESS);
}

// send a message to a specific device
bool RadioWrapper::sendTo(uint8_t* packet, uint8_t packetLength, uint8_t targetAddress) {
  return radioManager.sendto(packet, packetLength, targetAddress);
}

// receive the next packet on the buffer
bool RadioWrapper::receive(uint8_t* buffer, uint8_t* bufferSize, uint8_t* sourceAddress) {
  return radioManager.recvfrom(buffer, bufferSize, sourceAddress);
}

// setup the transceiver
bool RadioWrapper::setup(void (*errorCallback)(String)) {
  // reset the transceiver
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(10);
  digitalWrite(RESET_PIN, HIGH);
  delay(10);
  digitalWrite(RESET_PIN, LOW);
  delay(10);

  // initialize the radio manager
  if (!radioManager.init()) {
    errorCallback("Failed to initialize radio");
    return false;
  }

  // set the radio's frequency
  if (!radioDriver.setFrequency(frequency)) {
    errorCallback("Failed to set radio frequency to " + String(frequency) + "MHz");
    return false;
  }

  // configure the radio
  radioDriver.setTxPower(20, true);
  radioDriver.setEncryptionKey(Rf69Constants::encryptionKey);

  return true;
}

// no loop is needed for Rf69
void RadioWrapper::loop() {}
