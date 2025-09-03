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
XbeeWrapper radio(true); // XBee

// how long in milliseconds to accept data packets from sensors
// after this, wait for the gui to request more data before sending
// to avoid filling the gui's buffer
unsigned long packetAcceptanceIntervalMs = 100;
// tracks when last the gui requested data
unsigned long waitStartTime = 0;

// drive the radio transceiver
void loopRadio() {
  radio.loop();

  if (!radio.available()) return;

  // read in the packet
  uint8_t packetBuffer[MAX_PACKET_SIZE];
  uint8_t packetLength = sizeof(packetBuffer);
  if (!radio.receive(packetBuffer, &packetLength)) {
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

      // if not curretnly accepting data (gui still handling buffer usually)
      // then stop processing packet (drop it)
      if (millis() - waitStartTime > packetAcceptanceIntervalMs) break;

      if (packetLength < 3) {
        sendDebug("Data packet without all fields received");
        break;
      }

      // build a packet to send to gui in the for <coord data indicator> <sensor data>
      uint8_t coordinatorData[MAX_PACKET_SIZE];
      // mark as coordinator data packet
      coordinatorData[0] = COORDINATOR_DATA_INDICATOR;
      // put sensor data in packet
      memcpy(coordinatorData + 1, packetBuffer + 1, packetLength - 1);
      // send to GUI
      guiSerial.send(coordinatorData, packetLength);

      break;

    case DEBUG_PACKET_INDICATOR:
      // debug data to pass along to gui
      // mark with address of source device
      sendDebug(String((char*)(packetBuffer + 1)));
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
      // GUI is requesting data. forward all sensor data to the GUI
      // for a set interval
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

// runs once at startup
void setup() {
  // setup serial
  guiSerial.begin(SERIAL_BAUD_RATE);
  guiSerial.setPacketHandler(&onSerialPacketReceived);
  delay(10);

  if (!radio.setup(sendDebug)) {
    sendDebug("Radio setup failed");
    while (true);
  }
}

// runs on repeat
void loop() {
  // if listening for sensor data, check if time to stop forwarding sensor data
  if (millis() - waitStartTime > packetAcceptanceIntervalMs) {
    // let the GUI know the data packets finished
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
