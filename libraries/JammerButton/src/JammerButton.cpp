#include "JammerButton.h"

// constructor
JammerButton::JammerButton(
  uint8_t buttonPin,
  uint8_t routNumber
):
  buttonPin(buttonPin),
  routNumber(routNumber)
{}

void JammerButton::setJamHandler(void (*newJamHandler)(const uint8_t*, size_t, void*), void* context = nullptr) {
  packetHandler = newJamHandler;
  packetHandlerContext = context;
}

// setup the jammer button
void JammerButton::setup() {
  pinMode(buttonPin, INPUT);
}

// call frequently
// if jam button is pressed, send a jam packet
void JammerButton::loop() {
  if (digitalRead(buttonPin) == LOW) return;

  uint8_t jamPacket[5];
  jamPacket[0] = SENSOR_DATA_INDICATOR; // mark as data packet
  jamPacket[1] = JAM_DATA_INDICATOR;  // mark as jam packet
  jamPacket[2] = routNumber; // show that this device is the jammer
  jamPacket[3] = 0x00;  // jam all sensor types
  jamPacket[4] = 0x00;  // jam all sensor ids
  
  packetHandler(jamPacket, sizeof(jamPacket), packetHandlerContext);
}
