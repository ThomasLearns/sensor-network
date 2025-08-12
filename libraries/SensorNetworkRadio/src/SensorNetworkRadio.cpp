#include "SensorNetworkRadio.h"
#include <Arduino.h>

// constructor: populate values
SensorNetworkRadio::SensorNetworkRadio(
  uint8_t chipSelectPin,
  uint8_t interruptPin,
  uint8_t resetPin,
  uint8_t address,
  float frequency
):
  radioDriver(chipSelectPin, interruptPin),
  radioManager(radioDriver, address),
  frequency(frequency),
  resetPin(resetPin)
{}

// encryption key is static for all SensorNetworkRadios
uint8_t SensorNetworkRadio::encryptionKey[16] = {
  0x29, 0x2f, 0x07, 0x6b, 0x91, 0xf2, 0xa4, 0x02,
  0x60, 0xb5, 0x07, 0x88, 0x0f, 0x1e, 0x61, 0x72
};

// setup the radio
void SensorNetworkRadio::setup() {
  // reset the transceiver
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  delay(10);
  digitalWrite(resetPin, HIGH);
  delay(10);
  digitalWrite(resetPin, LOW);
  delay(10);

  // initialize the radio manager
  if (!radioManager.init()) {
    Serial.write(0);
    Serial.println("Failed to init radio");
    while (true); // block
  }

  // set the radio's frequency
  if (!radioDriver.setFrequency(frequency)) {
    Serial.write(0);
    Serial.println("Failed to set radio frequency to " + String(frequency));
    while (true); // block
  }

  // configure the radio
  radioDriver.setTxPower(20, true);
  radioDriver.setEncryptionKey(encryptionKey);
}

// drive the radio
void SensorNetworkRadio::loop() {
  if (!radioManager.available()) return;

  // read in the packet
  uint8_t packetBuffer[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t packetLength = sizeof(packetBuffer);
  uint8_t sourceAddress;
  if (!radioManager.recvfrom(packetBuffer, &packetLength, &sourceAddress)) {
    Serial.write(0);
    Serial.println("Failed to receive packet");
    return;
  }

  // empty rest of buffer
  for (uint8_t i = packetLength; i < RH_RF69_MAX_MESSAGE_LEN; i++) {
    packetBuffer[i] = 0;
  }

  // if nothing in packet, stop
  if (packetLength < 1) {
    Serial.write(0);
    Serial.println("Empty packet received");
    return;
  }

  // check first byte of packet to determine packet type
  switch (packetBuffer[0]) {
    case 0x00:
      // debug packet: write to serial with indicator of source address
      Serial.write(0);  // debug packet indicator
      Serial.print("[" + String(sourceAddress) + "]: ");
      Serial.write(packetBuffer + 1, packetLength - 1); // remove 0x00 from start
      break;

    case 0x03:
      // registration packet: handle registration
      if (registrationHandler) {
        registrationHandler(sourceAddress); // give address of registering device
      } else {
        // no handler
        Serial.write(0);
        Serial.println("Registration packet received, but no handler registered");
      }
      break;

    case 0x05:
      // data packet: pass to handler
      if (dataHandler && packetLength > 1) {
        // remove 0x05 before handling
        dataHandler(packetBuffer + 1, packetLength - 1);
      } else if (!dataHandler) {
        // no handler
        Serial.write(0);
        Serial.println("Data packet received, but no handler registered");
      } else if (packetLength <= 1) {
        // no data in data packet
        Serial.write(0);
        Serial.println("Data packet received, but it was empty");
      }
      break;

    default:
      // packet type not implemented. Write it in hex to serial in a debug packet
      Serial.write(0);
      Serial.print("Unrecognized packet: ");
      for (uint8_t i = 0; i < packetLength; i++) {
        if (packetBuffer[i] < 0x10) Serial.print("0");
        Serial.print(packetBuffer[i], HEX);
        if (i < packetLength - 1) {
          Serial.print(" ");
        } else {
          Serial.print("\n");
        }
      }
  }
}

// broadcast a discovery packet
void SensorNetworkRadio::sendDiscoveryPacket() {
  uint8_t packetBuffer[1] = { 0x02 };
  if (!radioManager.sendto(packetBuffer, sizeof(packetBuffer), RH_BROADCAST_ADDRESS)) {
    // failed to send
    Serial.write(0);
    Serial.println("Failed to send discovery packet");
  }
}

// send a data packet
void SensorNetworkRadio::sendDataPacket(uint8_t* buffer, uint8_t bufferLength, uint8_t destinationAddress) {
  // make sure there it is not too big (and enough room to prepend 0x05)
  if (bufferLength > RH_RF69_MAX_MESSAGE_LEN - 1) {
    Serial.write(0);
    Serial.println("Data packet to transmit is too long");
    return;
  }

  // prepend the data packet indicator byte
  uint8_t packetBuffer[RH_RF69_MAX_MESSAGE_LEN] = { 0x05 };
  memcpy(packetBuffer + 1, buffer, bufferLength);

  // send the data packet
  if (!radioManager.sendto(packetBuffer, bufferLength + 1, destinationAddress)) {
    // failed to send
    Serial.write(0);
    Serial.println("Failed to send data packet");
  }
}
