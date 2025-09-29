#include "UltrasonicSensor.h"

// constructor
UltrasonicSensor::UltrasonicSensor(
  uint8_t triggerPin,
  uint8_t echoPin,
  uint8_t routNumber,
  void (*handleData)(const uint8_t*, size_t) = nullptr
):
  // callback for passing collected sensor data
  handleData(handleData),
  // handle for ultrasonic sensor
  sensor(triggerPin, echoPin, ultrasonicSensorTimeoutUs),
  // sensor's id
  routNumber(routNumber)
{}

void UltrasonicSensor::loop() {
  // don't do anything if there is nothing to do with
  // any sensor data we may collect
  if (handleData == nullptr) return;

  // get data from sensor
  uint16_t distance = sensor.read();
  
  // our ultrasonics don't read higher than 4 meters. They do give results
  // that are >400 when they timeout however (I believe this is the library's fault).
  if (distance <= 400) {
    // build data packet
    uint8_t dataPacket[3 + sizeof(distance)];
    dataPacket[0] = SENSOR_DATA_INDICATOR;  // mark as data packet
    dataPacket[1] = DISTANCE_DATA_INDICATOR;  // mark as a distance data packet
    dataPacket[2] = routNumber;  // mark with address of sensor
    memcpy(dataPacket + 3, &distance, sizeof(distance)); // put data in packet

    // send data packet
    handleData(dataPacket, sizeof(dataPacket));
  }
}
