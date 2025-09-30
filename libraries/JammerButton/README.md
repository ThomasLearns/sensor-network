# Jammer Button

This library handles generating jam packets when the jam button is pressed.

## Importing JammerButton Into Your Sketch

Add the following lines to your `sketch.yaml` in the **libraries** section:

```yaml
- dir: ../../libraries/JammerButton
- dir: ../../libraries/PacketTypes
```

## Use

Construct the jammer with the pin the button is connected to and the jammer's ID.

You should call the `setup` and `loop` functions in their respective function in your main `.ino` file.

```c++
JammerButton jammer(pin, id);

void setup() {
  jammer.setup();
}

void loop() {
  jammer.loop();
}
```

### Sending Jam Packets

The JammerButton will generate jam packets. You should give it a callback to pass these packets to.

```c++
void setup() {
  // ...

  jammer.setJamHandler(handlerFunction);
  // OR, if the handler function isn't static, but has a static version:
  jammer.setJamHandler(Type::handlerFunction, &handler);
}
```
