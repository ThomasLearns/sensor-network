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

For non-coordinator XBee devices, it is important to flip the switch on the XBee shield to `DLINE` when uploading sketches and to `UART` when in use.

## Coordinator Program

The coordinator program is responsible for receiving sensor data wirelessly from other boards and forwarding it to the GUI.

### XBee Settings
Using XCTU, the following settings differ from the factory default:
- AP: 2
- BD: 6
- D7: 0

## Ultrasonic Program

The ultrasonic program is responsible for sending sensor data from ultrasonic sensors to the coordinator board.

### XBee Settings
Using XCTU, the following settings differ from the factory default:
- AP: 2
- BD: 7
- D7: 0

## Packet Types

Packet types are indicated by the value of the first byte. Different packet type indicator bytes are defined in the `PacketTypes` library.

### Debug Packet (Sensor -> Coordinator OR Coordinator -> GUI)
A debug message for the coordinator to forward to the GUI or for the GUI to handle as it sees fit.

`<Debug Packet Indicator Byte> <Message>`

### Coordinator Data Request (GUI -> Coordinator)
A message that the GUI is prepared to receive more data

`<Coordinator Data Request Indicator Byte>`

### Sensor Data (Sensor -> Coordinator)
A packet containing data from a sensor

`<Sensor Data Indicator Byte> <Data Type Indicator Byte> <Sensor ID Byte> <Data>`

#### Distance
Contains a distance measured in centimeters

`<Sensor Data Indicator Byte> <Data Type Indicator Byte> <Sensor ID Byte> <Distance (2 Bytes)>`

### Coordinator Data (Coordinator -> GUI)
Sensor data forwarded to the GUI

`<Coordinator Data Indicator Byte> <Data Type Indicator Byte> <Sensor ID Byte> <Data>`

Same as Sensor Data except with different indicator byte. These two types can reasonably be combined.

### GUI Handshake (GUI <-> Coordinator)
Used by the GUI to determine if a connected Arduino Uno is a coordinator. 
