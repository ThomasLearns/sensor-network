# sensor-network

Microcontroller code for transporting sensor data to the sensor GUI.

## Compiling

You will need `arduinio-cli` from [Arduino CLI Installation](https://docs.arduino.cc/arduino-cli/installation/#download).

To compile, run:
```shell
arduino-cli compile <path to program folder>
```

To select what radio you are compiling for, you must comment/uncomment the appropriate libraries in the `sketch.yaml` file as well as comment/uncomment the `#include` directives and radio constructors in the `.ino` files.

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

Also make sure it is using 802.15.4.

You can find a config file to do this on the Game of Drones USB drive or in this repo once/if we get Git LFS working.

## Ultrasonic Program

The ultrasonic program is responsible for sending sensor data from ultrasonic sensors to the coordinator board.

### XBee Settings
Using XCTU, the following settings differ from the factory default:
- AP: 2
- BD: 7
- D7: 0

Also make sure it is using 802.15.4.

You can find a config file to do this on the Game of Drones USB drive or in this repo once/if we get Git LFS working.

## Jammer Program

The jammer program is responsible for forming and sending jam packets

### XBee Settings
Use the same XBee Settings as the Ultrasonic Program.

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

`<Sensor Data Indicator Byte> <Distance Indicator Byte> <Sensor ID Byte> <Distance (2 Bytes)>`

#### Jam
Contains a packet with info on what to jam

For `Target Device Type ID Byte` and `Target Device ID Byte`, a value of `0x00` is used to represent all possible devices.

`<Sensor Data Indicator Byte> <Jam Indicator Byte> <Network ID Byte> <Target Device Type ID Byte> <Target Device ID Byte>`

### Coordinator Data (Coordinator -> GUI)
Sensor data forwarded to the GUI

`<Coordinator Data Indicator Byte> <Data Type Indicator Byte> <Sensor ID Byte> <Data>`

Same as Sensor Data except with different indicator byte. These two types can reasonably be combined.

### GUI Handshake (GUI <-> Coordinator)
Used by the GUI to determine if a connected Arduino Uno is a coordinator. 
