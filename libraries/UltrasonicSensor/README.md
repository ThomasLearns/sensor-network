# Ultrasonic Sensor

This library packages output from an ultrasonic sensor.

## Importing UltrasonicSensor Into Your Sketch

Add the following line to your `sketch.yaml` in the **libraries** section:

```yaml
- dir: ../../libraries/XbeeRadio
- XBee-Arduino library (0.6.0)
- AltSoftSerial (1.4.0)
```

## Use

You must provide the ultrasonic sensor's pins and ID.

You should call the `loop` method in its respective function in your main `.ino` file.

```c++
#include <UltrasonicSensor.h>

UltrasonicSensor sensor(triggerPin, echoPin);

// ...

void loop() {
  sensor.loop();
}
```

### Handling Sensor Data

The UltrasonicSensor generates packets. You can forward these packets to other components.

```c++
void setup() {
  // ...

  sensor.setDataHandler(handlerFunction);
  // OR, if the handler function is not static but has a static version:
  sensor.setDataHandler(Type::handlerFunction, &handler);
}
```
