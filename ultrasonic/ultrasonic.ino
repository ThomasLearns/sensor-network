#include <SensorNetworkRadio.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

// device address on network
#define ADDRESS 1

// MHz
#define RADIO_FREQUENCY 915.0

// transceiver pins
#define CHIP_SELECT_PIN 4
#define RADIO_INTERRUPT_PIN 3
#define RADIO_RESET_PIN 2

// transceiver settings
RH_RF69 radioDriver(CHIP_SELECT_PIN, RADIO_INTERRUPT_PIN);
RHReliableDatagram radioManager(radioDriver, ADDRESS);
void setupRadio() {
  // reset the transceiver
  pinMode(RADIO_RESET_PIN, OUTPUT);
  digitalWrite(RADIO_RESET_PIN, LOW);
  delay(10);
  digitalWrite(RADIO_RESET_PIN, HIGH);
  delay(10);
  digitalWrite(RADIO_RESET_PIN, LOW);
  delay(10);

  // initialize the radio manager
  if (!radioManager.init()) {
    Serial.println("Failed to init radio");
    while (true); // block
  } 
  
  // set the radio's frequency
  if (!radioDriver.setFrequency(RADIO_FREQUENCY)) {
    Serial.println("Failed to set radio frequency to " + String(RADIO_FREQUENCY) + "MHz");
    while (true); // block
  }

  // configure the radio
  radioDriver.setTxPower(20, true);
  radioDriver.setEncryptionKey(encryptionKey);
}

void loopRadio() {
  if (!radioManager.available()) return;

  // read in the packet
  uint8_t packetBuffer[RH_BROADCAST_ADDRESS];
  uint8_t packetLength = sizeof(packetBuffer);
  uint8_t sourceAddress;
  if (!radioManager.recvfrom(packetBuffer, &packetLength, &sourceAddress)) {
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

      // temporary testing data to be removed when sensor is implemented
      uint8_t data[2] = { 0x00, 0xC8 };

      // build data packet
      uint8_t dataPacket[4 + sizeof(data)];
      dataPacket[0] = SENSOR_DATA_INDICATOR;  // mark as data packet
      dataPacket[1] = packetBuffer[1];  // mark with request id
      dataPacket[2] = DISTANCE_DATA_INDICATOR;  // mark as a distance data packet
      dataPacket[3] = ADDRESS;  // mark with address of sensor
      memcpy(dataPacket + 4, data, sizeof(data)); // put data in packet

      // send data packet to sender of data request
      radioManager.sendto(dataPacket, sizeof(dataPacket), sourceAddress);
      break;

    default:
      Serial.println("Unrecognized packet type");
  }
}

// runs at startup
void setup() {
  // setup serial
  Serial.begin(9600);
  while (!Serial);

  setupRadio();
}

// runs on repeat
void loop() {
  // used to access the time of this loop
  unsigned long currentTimeMs = millis();

  // drive the radio
  loopRadio();
}
