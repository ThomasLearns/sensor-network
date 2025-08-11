# sensor-network

Microcontroller code for transporting sensor data to the sensor GUI.

## Compiling

To compile, run:
```shell
arduino-cli compile <path to program folder> --libraries <absolute path to libraries folder> -b arduino:avr:uno
```

When arduino-cli reaches v1.3+, rename files from `.sketch.yaml` to `sketch.yaml`. After doing so, the compilation command should simply be `arduino-cli compile <path>`

## Uploading

To upload a compiled program to a board, use `arduino-cli board list` to see the ports of connected boards, then use:
```shell
arduino-cli upload -p <port> <path to program folder>
```
## Coordinator Program

The coordinator program is responsible for receiving sensor data wirelessly from other boards and forwarding it to the GUI.

## Ultrasonic Program

The ultrasonic program is responsible for sending sensor data from ultrasonic sensors to the coordinator board.
