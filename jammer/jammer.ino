#include <PacketTypes.h>

// select radio to use
// #include <Rf69Wrapper.h>
// OR
#include <XbeeWrapper.h>

// device address on network
#define ROUT_NUMBER 9

// MHz
#define RADIO_FREQUENCY 915.0

// pin used to select the main transeiver
#define CHIP_SELECT_PIN 4

// object used to communicate with coordinator
// Rf69Wrapper radio(ROUT_NUMBER, CHIP_SELECT_PIN, RADIO_FREQUENCY); // RF69
XbeeWrapper radio;  // XBee

// the jam button
#define JAM_BUTTON_PIN A0

// send messages to coordinator for it to forward to gui which treats it as debug info
// because we can't necessarily use serial from a sensor.
void sendDebug(String message) {
  String messageWithRout = "[" + String(ROUT_NUMBER) + "]: " + message;
  uint8_t* cString = messageWithRout.c_str();

  uint8_t buffer[MAX_PACKET_SIZE];
  buffer[0] = DEBUG_PACKET_INDICATOR;
  memcpy(buffer + 1, cString, min(MAX_PACKET_SIZE - 1, strlen(cString)));

  radio.sendToCoordinator(buffer, min(MAX_PACKET_SIZE, strlen(cString) + 1));
}

void handleRadioSetupError(String error) {
  sendDebug(error);
}

// runs at startup
void setup() {
  pinMode(JAM_BUTTON_PIN, INPUT);

  if (!radio.setup(handleRadioSetupError)) {
    sendDebug("Radio setup failed");
    while (true) {} // block
  }
}

// runs on repeat
void loop() {
  // get if button is pressed
  if (digitalRead(JAM_BUTTON_PIN)) {
    uint8_t jamPacket[5];
    jamPacket[0] = SENSOR_DATA_INDICATOR; // mark as data packet
    jamPacket[1] = JAM_DATA_INDICATOR;  // mark as jam packet
    jamPacket[2] = ROUT_NUMBER; // show that this device is the jammer
    jamPacket[3] = 0x00;  // jam all sensor types
    jamPacket[4] = 0x00;  // jam all sensor ids
    
    radio.sendToCoordinator(jamPacket, sizeof(jamPacket));
  }

  radio.loop();
}
