#include <JammerButton.h>
#include <XbeeRadio.h>

// send jam packets over the XBee network when the jam button is pressed

// sensor id
#define routNumber 0

// pin used for jam button
#define jamButtonPin A0

JammerButton jammer(
  jamButtonPin,
  routNumber
);

void setup() {
  xbee::setup();
  jammer.setup();
  jammer.setJamHandler(xbee::sendToCoordinator);
}

void loop() {
  xbee::loop();
  jammer.loop();
}
