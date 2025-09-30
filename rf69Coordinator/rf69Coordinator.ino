#include <GuiConnection.h>
#include <Rf69Radio.h>

// forward information received from a RF69 radio module to the GUI

Rf69Radio radio;

void setup() {
  gui::setup();

  radio.setup();
  // have the gui handle incoming packets
  radio.setPacketHandler(gui::handlePacket);
}

void loop() {
  gui::loop();
  radio.loop();
}
