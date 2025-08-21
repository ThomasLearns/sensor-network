#include <PacketTypes.h>
#include <PacketSerial.h>
// choose between a radio to use
// #include <Rf69Wrapper.h>
#include <XbeeWrapper.h>

// serial protocol for communicating with GUI
SLIPPacketSerial guiSerial;
// because PacketSerial needs complete control over Serial,
// we have to go through it to log debug information.
#define MAX_DEBUG_LENGTH 100  // messages with more characters will be truncated.
void sendDebug(String message) {
  // convert the message to a char[]
  uint8_t* messageCString = message.c_str();

  // build the debug packet
  uint8_t buffer[MAX_DEBUG_LENGTH + 1];
  buffer[0] = DEBUG_PACKET_INDICATOR; // used to indicate this packet is not program data
  memcpy(buffer + 1, messageCString, min(MAX_DEBUG_LENGTH, strlen(messageCString)));
  
  // send on serial
  guiSerial.send(buffer, min(MAX_DEBUG_LENGTH + 1, strlen(messageCString) + 1));
}
// baud rate for serial communication (communication with GUI)
#define SERIAL_BAUD_RATE 115200


// device address
// should be different for each device on network
#define ADDRESS 0
// transeiver pins
#define CHIP_SELECT_PIN 4
// MHz
#define RADIO_FREQUENCY 915.0

// radio interaction object
// Rf69Wrapper radio(ADDRESS, CHIP_SELECT_PIN, RADIO_FREQUENCY); // RF69
XbeeWrapper radio(ADDRESS); // XBee

// after sending out a data request, we wait to ensure all data is
// received
unsigned long dataIdChangeIntervalMs = 2;
unsigned long lastPacketMs = 0;
// how much interval decreases each time
unsigned long dataIdIntervalDecreaseRate = 1; // ms
// how much interval increases when a packet is missed
unsigned long dataIdIntervalIncreaseRate = 5;
// how much time to leave after the last packet to avoid missing it in the future
unsigned long dataIdIntervalSpacer = 5;  // ms
bool waitingForData = false;
unsigned long waitStartTime = 0;
// rotating id to be able to tell if a data packet is old
uint8_t currentDataId = 0x00;

// drive the radio transceiver
void loopRadio() {
  radio.loop();

  if (!radio.available()) return;

  // read in the packet
  uint8_t packetBuffer[MAX_PACKET_SIZE];
  uint8_t packetLength = sizeof(packetBuffer);
  uint8_t sourceAddress;
  if (!radio.receive(packetBuffer, &packetLength, &sourceAddress)) {
    sendDebug("Failed to receive packet");
    return;
  }

  // if nothing in packet, stop
  if (packetLength < 1) {
    sendDebug("Empty packet received");
    return;
  }

  // handle the packet depending on its type (determined by first byte)
  switch (packetBuffer[0]) {
    case SENSOR_DATA_INDICATOR:
      // data from sensor received.
      // now process slightly and send to GUI

      // don't bother with this packet if its out of date
      if (packetBuffer[1] != currentDataId) {
        dataIdChangeIntervalMs += dataIdIntervalIncreaseRate;
        break;
      }

      // mark now as the last time a packet matching data id
      // was received
      lastPacketMs = millis();

      // build a packet to send to gui in the for <coord data indicator> <sensor data>
      uint8_t coordinatorData[MAX_PACKET_SIZE];
      // mark as coordinator data packet
      coordinatorData[0] = COORDINATOR_DATA_INDICATOR;
      // put sensor data in packet
      memcpy(coordinatorData + 1, packetBuffer + 2, packetLength - 2);
      // send to GUI
      guiSerial.send(coordinatorData, packetLength - 1);
      break;

    case DEBUG_PACKET_INDICATOR:
      // debug data to pass along to gui
      // mark with address of source device
      sendDebug("[" + String(sourceAddress) + "]: " + String((char*)(packetBuffer + 1)));
      break;

    default:
      sendDebug("Unrecognized packet type");
  }
}

// runs when serial receives data
void onSerialPacketReceived(const uint8_t* buffer, size_t size) {
  if (size < 1) {
    sendDebug("Received serial packet, but it was empty");
    return;
  }

  // check first byte of packet to determine type
  switch(buffer[0]) {
    case COORDINATOR_DATA_REQUEST_INDICATOR:
      // GUI is requesting data. broadcast a data request to sensors

      // build request
      uint8_t sensorDataRequest[2];
      sensorDataRequest[0] = SENSOR_DATA_REQUEST_INDICATOR;
      sensorDataRequest[1] = currentDataId;

      // send request
      radio.broadcast(sensorDataRequest, sizeof(sensorDataRequest));

      // start paying attention to data packets coming in
      waitingForData = true;
      waitStartTime = millis();
      break;

    case GUI_HANDSHAKE_INDICATOR:
      // form connection with gui by returning handshake
      guiSerial.send(buffer, 1);
      break;

    default:
      sendDebug("Unrecognized serial packet");
  }
}

void showRadioSetupError(String message) {
  sendDebug(message);
}

// runs once at startup
void setup() {
  // setup serial
  guiSerial.begin(SERIAL_BAUD_RATE);
  guiSerial.setPacketHandler(&onSerialPacketReceived);
  delay(10);

  if (!radio.setup(showRadioSetupError)) {
    sendDebug("Radio setup failed");
    while (true);
  }
}

// runs on repeat
void loop() {
  // if listening for sensor data, check if time to stop listening (and cycle data id)
  if (waitingForData && millis() - waitStartTime >= dataIdChangeIntervalMs) {
    currentDataId++;  // cycle data id so we can differentiate between old and new packets
    waitingForData = false; // stop caring about sensor data packets

    // decrease the data id change interval to prevent it from being too long
    // (only if we have packets to go off of, as there would be no point in decreasing time)
    if (lastPacketMs >= waitStartTime) {
      unsigned long newDataIdInterval = lastPacketMs - waitStartTime + dataIdIntervalSpacer;

      // decrease interval to tighten timing (if possible)
      if (dataIdChangeIntervalMs > dataIdIntervalDecreaseRate) {
        if (newDataIdInterval < dataIdChangeIntervalMs - dataIdIntervalDecreaseRate) {
          // cap decrease to set rate
          newDataIdInterval = dataIdChangeIntervalMs - dataIdIntervalDecreaseRate;
        } else if (newDataIdInterval < 1) {
          // don't go lower than 1
          newDataIdInterval = 1;
        }
      }

      dataIdChangeIntervalMs = newDataIdInterval;
    }

    // send a packet to the gui to let it know that we are done with this cycle of data
    uint8_t dataDone[1] = { COORDINATOR_DATA_DONE_INDICATOR };
    guiSerial.send(dataDone, sizeof(dataDone));
  }

  // drive the radio
  loopRadio();

  // drive serial
  guiSerial.update();
  if (guiSerial.overflow()) {
    sendDebug("Serial is overflowing");
  }
}
