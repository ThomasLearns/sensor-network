#include <PacketTypes.h>
#include <Ultrasonic.h>

// select radio to use
// #include <Rf69Wrapper.h>
// OR
#include <XbeeWrapper.h>

// device address on network
#define ADDRESS 1

// MHz
#define RADIO_FREQUENCY 915.0

// pin used to select the main transeiver
#define CHIP_SELECT_PIN 4

// object used to communicate with coordinator
// Rf69Wrapper radio(ADDRESS, CHIP_SELECT_PIN, RADIO_FREQUENCY); // RF69
XbeeWrapper radio(ADDRESS);  // XBee

// the ultrasonic sensor
#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define SENSOR_TIMEOUT_US 24000UL
Ultrasonic sensor(TRIGGER_PIN, ECHO_PIN, SENSOR_TIMEOUT_US);

// call every loop
// listens to packets from coordinator
void loopRadio() {
  // handle internal actions needed every loop
  radio.loop();

  // stop if no available packets to read
  if (!radio.available()) return;

  // read in the packet
  uint8_t packetBuffer[MAX_PACKET_SIZE];
  uint8_t packetLength = sizeof(packetBuffer);
  uint8_t sourceAddress;
  if (!radio.receive(packetBuffer, &packetLength, &sourceAddress)) {
    Serial.println("Failed to receive packet");
    return;
  }

  // if nothing in packet, stop
  if (packetLength < 1) {
    Serial.println("Empty packet received");
    return;
  }

  // determine the type of packet by the first byte
  switch (packetBuffer[0]) {
    case SENSOR_DATA_REQUEST_INDICATOR:
      // data requests must have a 2nd byte for the request id
      if (packetLength < 2) {
        Serial.println("Received data request without an ID");
        break;
      }

      { // create scope to allow creating variables
      // temporary testing data to be removed when sensor is implemented
      uint16_t data = sensor.read();

      // build data packet
      uint8_t dataPacket[4 + sizeof(data)];
      dataPacket[0] = SENSOR_DATA_INDICATOR;  // mark as data packet
      dataPacket[1] = packetBuffer[1];  // mark with request id
      dataPacket[2] = DISTANCE_DATA_INDICATOR;  // mark as a distance data packet
      dataPacket[3] = ADDRESS;  // mark with address of sensor
      memcpy(dataPacket + 4, &data, sizeof(data)); // put data in packet

      // send data packet to sender of data request
      radio.sendTo(dataPacket, sizeof(dataPacket), sourceAddress);
      }

      break;

    default:
      Serial.println("Unrecognized packet type");
  }
}

void handleRadioSetupError(String error) {
  Serial.println(error);
}

// runs at startup
void setup() {
  // setup serial
  Serial.begin(9600);
  while (!Serial);
  
  if (!radio.setup(handleRadioSetupError)) {
    Serial.println("Radio setup failed");
    while (true) {} // block
  }
}

// runs on repeat
void loop() {
  loopRadio();
}
