#ifndef GUI_CONNECTION
#define GUI_CONNECTION

#include <PacketSerial.h>
#include <PacketTypes.h>

// add the following entries to the libraries list in your sketch.yaml
// - dir: ../../libraries/GuiConnection
// - dir: ../../libraries/PacketTypes
// - PacketSerial (1.4.0)

namespace gui {
  // how long in milliseconds to accept data packets from sensors
  // after this, wait for the gui to request more data before sending
  // to avoid filling the gui's buffer
  #define packetAcceptanceIntervalMs 100

  // needs to be set the same here and on GUI program
  #define guiBaudRate 115200

  // max bytes to send as debug/data packet
  #define MAX_DEBUG_LENGTH 100
  #define MAX_PACKET_SIZE 100

  // should be called from corresponding functions in main file
  void setup();
  void loop();

  // used to send debug info to the gui
  void sendDebug(String);
  void sendDebug(uint8_t*, size_t);

  // used to forward packets from the network to the gui
  // if forceSend is true, applicable packet types will be sent to the GUI
  // even if the GUI isn't ready. Use forceSend minimally.
  void handlePacket(uint8_t*, size_t, bool forceSend);

}

#endif
