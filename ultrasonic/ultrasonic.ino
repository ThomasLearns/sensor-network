#include <PacketTypes.h>
#include <Ultrasonic.h>

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

// the ultrasonic sensor
#define TRIGGER_PIN 12 // 9 on Rf69
#define ECHO_PIN 11  // 8 on Rf69
#define SENSOR_TIMEOUT_US 24000UL
Ultrasonic sensor(TRIGGER_PIN, ECHO_PIN, SENSOR_TIMEOUT_US);

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
  if (!radio.setup(handleRadioSetupError)) {
    sendDebug("Radio setup failed");
    while (true) {} // block
  }
}

// runs on repeat
void loop() {
  // get distance from sensor
  uint16_t data = sensor.read();

  // our ultrasonics don't read higher than 4 meters. They do give results
  // that are >400 when they timeout however (I believe this is the library's fault).
  if (data <= 400) {
    // build data packet
    uint8_t dataPacket[3 + sizeof(data)];
    dataPacket[0] = SENSOR_DATA_INDICATOR;  // mark as data packet
    dataPacket[1] = DISTANCE_DATA_INDICATOR;  // mark as a distance data packet
    dataPacket[2] = ROUT_NUMBER;  // mark with address of sensor
    memcpy(dataPacket + 3, &data, sizeof(data)); // put data in packet

    // send data packet to sender of data request
    radio.sendToCoordinator(dataPacket, sizeof(dataPacket));
  }

  radio.loop();
}
