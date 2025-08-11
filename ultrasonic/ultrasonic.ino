
// time between sending packets
unsigned long sendIntervalMs = 10;
// time of last packet sent
unsigned long lastSendMs = 0;

// send a packet to the coordinator
void sendPacket() {
  // mark now as the last time a packet was sent
  lastSendMs = millis()
}

// runs at startup
void setup() {
  // send the first packet
  sendPacket()
}

// runs on repeat
void loop() {
  // used to access the time of this loop
  unsigned long currentTimeMs = millis();

  // if enough time has passed, send a packet to the coordinator
  if (currentTimeMs - lastSendMs >= sendIntervalMs) {
    sendPacket();
  }
}
