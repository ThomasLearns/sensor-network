#include "XbeeRadio.h"

namespace xbee {
  XBeeWithCallbacks xbee;
  AltSoftSerial xbeeSerial; // software serial

  // pointer to callback for handling received packets
  void (*onPacketReceived)(const uint8_t*, size_t);

  // open packets and pass to handler
  void packetHandler(Rx16Response& response, uintptr_t) {
    onPacketReceived(response.getData(), response.getDataLength());
  }

  void setup(
    void (*finalPacketHandler)(const uint8_t*, size_t),
    bool useSoftwareSerial = false
  ) {
    // use this as the handler function for received packets (after opening with packetHandler())
    onPacketReceived = finalPacketHandler;

    // setup the correct type of serial
    if (useSoftwareSerial) {
      // Note: using software serial. Ensure switch on xbee shield is set to DLINE
      xbeeSerial.begin(xbeeSoftwareSerialBaudRate);
      xbee.begin(xbeeSerial);
    } else {
      // !!!WARNING!!!
      // using harware serial. Uploading again after this requires the switch
      // on the arduino to be on DLINE. When in use, set switch to UART.
      // No other components of this program can be allowed to use Serial.
      Serial.end();
      Serial.begin(xbeeHardwareSerialBaudRate);
      xbee.setSerial(Serial);
    }
    delay(1);

    // handle received packets
    xbee.onRx16Response(packetHandler);
  }

  void loop() {
    // drive the xbee module
    xbee.loop();
  }

  // send a packe to the coordinator
  void sendToCoordinator(uint8_t* packet, size_t packetSize) {
    // coordinator's address is 0x0000
    Tx16Request txRequest(0x0000, packet, packetSize);
    txRequest.setOption(0x01);  // disable ack

    // send packet
    xbee.send(txRequest);
  }
};
