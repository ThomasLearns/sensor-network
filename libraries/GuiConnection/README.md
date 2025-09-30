# GUI Connection

This library processes network packets to send to the GUI and handles packets from the GUI.

## Importing GuiConnection Into Your Sketch

Add the following lines to your `sketch.yaml` in the **libraries** section:

```yaml
- dir: ../../libraries/GuiConnection
- dir: ../../libraries/PacketTypes
- PacketSerial (1.4.0)
```

## Use

You should call the `setup` and `loop` functions in their respective function in your main `.ino` file.

```c++
void setup() {
  gui::setup();
}

void loop() {
  gui::loop();
}
```

### Handling Network Packets

The GuiConnection library is meant to have packets passed to it. It will then process and forward these packets to the GUI

```c++
// pass packets to jammer
radio.setPacketHandler(gui::handlePacket);
```
