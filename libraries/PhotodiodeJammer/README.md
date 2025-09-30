# Photodiode Jammer

This library defines an object that blocks packets when a LASER hits a photodiode array at the correct frequency.

## Importing PhotodiodeJammer Into Your Sketch

Add the following line to your `sketch.yaml` in the **libraries** section:

```yaml
- dir: ../../libraries/PhotodiodeJammer
```

## Use

You must provide the photodiode array's pin and the target frequency.

You can use any float for the target frequency, but you should use `laserLow`, `laserMedium`, or `laserHigh` as these are set to correspond to the three frequencies the RC Jammer is capable of.

You should call the `setup` and `loop` methods in their respective function in your main `.ino` file.

```c++
PhotodiodeJammer jammer(pin, frequency);

void setup() {
  jammer.setup();
}

void loop() {
  jammer.loop();
}
```

### Jamming

The PhotodiodeJammer object is meant to have packets passed to it. It will then forward these packets to another component **only** if the device is not being jammed.

```c++
// pass packets to jammer
sensor.setDataHandler(PhotodiodeJammer::pipeJammablePacket, &jammer);

// the jammer passes unjammed packets to a handler of your choosing
jammer.setUnjammedPacketHandler(packetHandler);
// OR
jammer.setUnjammedPacketHandler(ObjectExampleType::packetHandler, &objectExample);
```
