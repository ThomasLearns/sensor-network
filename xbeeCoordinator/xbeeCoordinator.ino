#include <GuiConnection.h>
#include <XbeeRadio.h>

// Parse packets from the xbee network and communicate with the GUI.

// This sketch uses software serial to communicate with the XBee module.
// Make sure the switch on the XBee shield is set to DLINE and the pins
// 8 and 9 are connected to 2 and 3 respectively.

void setup() {
  gui::setup();
  // have the xbee module forward received packets to the gui connection
  // "true" means to use software serial
  xbee::setup(gui::handlePacket, true);
}

void loop() {
  gui::loop();
  xbee::loop();
}
