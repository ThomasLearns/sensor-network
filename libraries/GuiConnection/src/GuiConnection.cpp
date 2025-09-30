#include "GuiConnection.h"

// serial protocol for communicating with GUI
SLIPPacketSerial guiSerial;

// tracks when last the gui requested data
unsigned long waitStartTime = 0;

void onSerialPacketReceived(const uint8_t* buffer, size_t size) {
  
  // ensure packet has content
  if (size < 1) {
    gui::sendDebug("Received serial packet, but it was empty");
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
      gui::sendDebug("Unrecognized serial packet");
  }
}

// should be called from main setup function
void gui::setup() {
  // set up the serial connection
  guiSerial.begin(guiBaudRate);
  guiSerial.setPacketHandler(&onSerialPacketReceived);
  delay(10);
}

// should be called every loop
void gui::loop() {
  // if enough time has passed since gui sent data request,
  // tell gui data is done being sent
  if (millis() - waitStartTime > packetAcceptanceIntervalMs) {
    uint8_t dataDone[1] = { COORDINATOR_DATA_DONE_INDICATOR };
    guiSerial.send(dataDone, sizeof(dataDone));
  }

  // drive the serial connection
  guiSerial.update();
  if (guiSerial.overflow()) {
    sendDebug("Serial is overflowing");
  }
}

// send a debug message to the gui
void gui::sendDebug(String message) {
  // convert the message to a char[]
  uint8_t* messageCString = message.c_str();
  sendDebug(messageCString, strlen(messageCString));
}
void gui::sendDebug(uint8_t* message, size_t messageLength) {

  // build the debug packet
  uint8_t buffer[MAX_DEBUG_LENGTH + 1];
  buffer[0] = DEBUG_PACKET_INDICATOR; // used to indicate this packet is not program data
  memcpy(buffer + 1, message, min(MAX_DEBUG_LENGTH, messageLength));
  
  // send on serial
  guiSerial.send(buffer, min(MAX_DEBUG_LENGTH + 1, messageLength + 1));
}

// determine if/how to send data to the gui
void gui::handlePacket(uint8_t* data, size_t dataLength, bool forceSend = false) {
  if (dataLength < 1) {
    sendDebug("Empty packet received");
    return;
  }

  // first byte of packet is packet type
  switch (data[0]) {
    case SENSOR_DATA_INDICATOR:
      // data from sensor received.
      // now process slightly and send to GUI

      // if not curretnly accepting data (gui still handling buffer usually)
      // then stop processing packet (drop it)
      // send the data anyway if the forceSend flag is set
      if (!forceSend && millis() - waitStartTime > packetAcceptanceIntervalMs) break;

      if (dataLength < 3) {
        sendDebug("Data packet without all fields received");
        break;
      } else if (dataLength > MAX_PACKET_SIZE) {
        sendDebug("Too much data to send to GUI");
        break;
      }

      // build a packet to send to gui in the for <coord data indicator> <sensor data>
      uint8_t guiData[MAX_PACKET_SIZE];
      // mark as coordinator data packet
      guiData[0] = COORDINATOR_DATA_INDICATOR;
      // put sensor data in packet
      memcpy(guiData + 1, data + 1, dataLength - 1);
      // send to GUI
      guiSerial.send(guiData, dataLength);

      break;

    default:
      sendDebug("Unsupported packet type");
  }
}
