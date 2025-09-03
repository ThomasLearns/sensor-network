#include "XbeeWrapper.h"
#include <XBee.h>
#include <AltSoftSerial.h>

// constructor
// initialize xbee, determine what type of serial to use, and prepare response holding variable
XbeeWrapper::XbeeWrapper(bool useSoftwareSerial = false):
  useSoftwareSerial(useSoftwareSerial)
{
  xbee = XBee();
  rx = Rx16Response();
}

// check if a packet is available
bool XbeeWrapper::available() {
  // determine if the module is receiving a packet
  return xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == RX_16_RESPONSE;
}

// broadcast a packet to all devices on the network
bool XbeeWrapper::broadcast(uint8_t* packet, uint8_t packetLength) {
  // prepare the packet
  Tx16Request txRequest(0xFFFF, packet, packetLength);
  txRequest.setOption(0x01);
  
  // send the packet
  xbee.send(txRequest);

  return true;
}

// send a packet to a specified device
bool XbeeWrapper::sendToCoordinator(uint8_t* packet, uint8_t packetLength) {
  // prepare the packet
  Tx16Request txRequest(0x0000, packet, packetLength);
  txRequest.setOption(0x01);

  // send the packet
  xbee.send(txRequest);

  return true;
}

// receive a packet
bool XbeeWrapper::receive(uint8_t* buffer, uint8_t* bufferSize) {
  xbee.getResponse().getRx16Response(rx);

  // copy over packet data
  memcpy(buffer, rx.getData(), min(*bufferSize, rx.getDataLength()));
  *bufferSize = rx.getDataLength();

  return true;
}

// setup the xbee
bool XbeeWrapper::setup(void (*handleError)(String)) {
  // setup a software serial to communicate with the xbee

  if (useSoftwareSerial) {
    // set up the software serial at its fastest reliable baud rate
    xbeeSerial.begin(57600);
    xbee.begin(xbeeSerial);
  } else {
    // use hardware serial (it is important that this
    // radio is the only device using hardware serial)
    Serial.end();
    Serial.begin(115200);
    xbee.setSerial(Serial);
  }
  delay(1);

  return true;
}

// drive the xbee module
void XbeeWrapper::loop() {
  // get the next packet (if any)
  do {
    xbee.readPacket();
  } while (xbee.getResponse().isAvailable() && !available());
}
