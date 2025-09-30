# XBee Radio

This library handles reading from and writing to an XBee network

## Importing XbeeRadio Into Your Sketch

Add the following line to your `sketch.yaml` in the **libraries** section:

```yaml
- dir: ../../libraries/XbeeRadio
- XBee-Arduino library (0.6.0)
- AltSoftSerial (1.4.0)
```

### Use

You should call XbeeRadio's `setup` and `loop` functions in the respective functions in your main `.ino` file.

```c++
#include <XbeeRadio.h>

void setup() {
  xbee::setup();
}

void loop() {
  xbee::loop();
}
```

### Receiving

If you want to forward received packets to a handler function, you should call `setPacketHandler` in `setup()`.

```c++
void packetHandler(const uint8_t* packet, size_t packetSize) {
  // ...
}

void setup() {
  radio.setup(packetHandler);
}
```

### Sending

If you want to send a packet to the coordinator, you should call `sendToCoordinator`.

```c++
uint8_t packet[] = { 0x00, 0x01, 0x02 };
radio.sendToCoordinator(packet, sizeof(packet));
```

#### Sending as a callback

If you want to pass the function for sending packets to the coordinator as an argument, you should use the static form.

```c++
setCallback(xbee::sendToCoordinator);
```
