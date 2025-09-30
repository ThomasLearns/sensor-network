# Rf69Radio

This library handles communication between devices using RF69 radio transceivers.

## Importing Rf69Radio Into Your Sketch
Make sure that in the **libraries** section of your `sketch.yaml`, you have the following entries:
```yaml
- dir: ../../libraries/Rf69Radio
- RadioHead (1.143.1)
```

## Use

After creating your `Rf69Radio` object, you should call its `setup` and `loop` member functions in the respecitve functions in your main `.ino` file.

```c++
Rf69Radio radio;

void setup() {
  radio.setup();
}

void loop() {
  radio.loop();
}
```

### Receiving

If you want to forward received packets to a handler function, you should call `setPacketHandler` in `setup()`.

```c++
void packetHandler(const uint8_t* packet, size_t packetSize) {
  // ...
}

void setup() {
  radio.setup();
  radio.setPacketHandler(packetHandler);
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
setCallback(Rf69Radio::sendToCoordinator, &radio);
```
