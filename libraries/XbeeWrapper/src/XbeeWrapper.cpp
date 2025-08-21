#include "XbeeWrapper.h"
#include <XBee.h>


// constructor
// set address, initialize xbee, and prepare response holding variable
XbeeWrapper::XbeeWrapper(uint8_t address): address(address) {
  xbee = XBee();
  rx = ZBRxResponse();
}

// check if a packet is available
bool XbeeWrapper::available() {
  // determine if the module is receiving a packet
  bool packetExists = xbee.getResponse().isAvailable()
    && xbee.getResponse().getApiId() == ZB_RX_RESPONSE;
  
  // no packet
  if (!packetExists) return false;

  // check the packets destination address to see if it is intended for this module
  xbee.getResponse().getZBRxResponse(rx);
  return rx.getDataLength() > 2 
    && (rx.getData()[1] == address  // this module's address
    || rx.getData()[1] == BROADCAST_ADDRESS);  // broadcast address
}

// broadcast a packet to all devices on the network
bool XbeeWrapper::broadcast(uint8_t* packet, uint8_t packetLength) {
  return sendTo(packet, packetLength, BROADCAST_ADDRESS);
}

// send a packet to a specified device
bool XbeeWrapper::sendTo(uint8_t* packet, uint8_t packetLength, uint8_t targetAddress) {
  // Because the ZigBee addressing has too many limitations, we handle addressing in software.
  // This means from the ZigBee side of things each packet is broadcasted.
  // set the destination 64 bit address to broadcast
  XBeeAddress64 serialNumber(0x00000000, 0x0000FFFF);

  // create another buffer so we can prepend addressing info to the packet
  uint8_t* buffer = new uint8_t[packetLength + 2*sizeof(address)];
  memcpy(buffer, &address, sizeof(address));  // source address
  memcpy(buffer + sizeof(address), &targetAddress, sizeof(address));  // destination address
  memcpy(buffer + 2*sizeof(address), packet, packetLength); // data 

  // prepare the packet
  ZBTxRequest txRequest(serialNumber, buffer, packetLength + 2*sizeof(address));
  
  // use broadcasting because we are handling addressing ourself
  txRequest.setAddress16(ZB_BROADCAST_ADDRESS);

  // send the packet
  xbee.send(txRequest);

  // clear memory, hopefully no fragmentation occurred since we only allocated
  // memory for a short time
  delete[] buffer;

  return true;
}

// receive a packet
bool XbeeWrapper::receive(uint8_t* buffer, uint8_t* bufferSize, uint8_t* sourceAddress) {
  xbee.getResponse().getZBRxResponse(rx);

  // copy over packet data (excluding the 2 addressing bytes)
  memcpy(buffer, rx.getData() + 2*sizeof(address), min(*bufferSize, rx.getDataLength() - 2*sizeof(address)));
  *bufferSize = rx.getDataLength() - 2*sizeof(address);
  memcpy(sourceAddress, rx.getData(), sizeof(address));

  return true;
}

// setup the xbee
bool XbeeWrapper::setup(void (*handleError)(String)) {
  // setup a software serial to communicate with the xbee
  xbeeSerial.begin(9600);
  xbee.begin(xbeeSerial);
  delay(1);

  return true;
}

// drive the xbee module
void XbeeWrapper::loop() {
  // get the next packet (if any)
  xbee.readPacket();
}
