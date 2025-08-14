#ifndef SENSOR_NETWORK_RADIO
#define SENSOR_NETWORK_RADIO

// indicators for types of packets in the GUI/Coordinator/Sensor network
#define DEBUG_PACKET_INDICATOR 0x00
#define COORDINATOR_DATA_REQUEST_INDICATOR 0x01
#define SENSOR_DATA_REQUEST_INDICATOR 0x02
#define SENSOR_DATA_INDICATOR 0x03
#define COORDINATOR_DATA_INDICATOR 0x04
#define COORDINATOR_DATA_DONE_INDICATOR 0x05
#define GUI_HANDSHAKE_INDICATOR 0x06

// indicators for types of data packets
#define DISTANCE_DATA_INDICATOR 0x00

// defined here so it can be the same on all devices in our network
uint8_t encryptionKey[16] = {
  0x29, 0x2f, 0x07, 0x6b, 0x91, 0xf2, 0xa4, 0x02,
  0x60, 0xb5, 0x07, 0x88, 0x0f, 0x1e, 0x61, 0x72
};

#endif
