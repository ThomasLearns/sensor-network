#include <PacketTypes.h>
#include <Ultrasonic.h>

// select radio to use
#include <Rf69Wrapper.h>
// OR
// #include <XbeeWrapper.h>

// device address on network
#define ROUT_NUMBER 9

// MHz
#define RADIO_FREQUENCY 915.0

// pin used to select the main transeiver
#define CHIP_SELECT_PIN 4

// object used to communicate with coordinator
Rf69Wrapper radio(ROUT_NUMBER, CHIP_SELECT_PIN, RADIO_FREQUENCY); // RF69
// XbeeWrapper radio;  // XBee

// the ultrasonic sensor
#define TRIGGER_PIN 9 //12 // 9 on Rf69
#define ECHO_PIN 8 //11  // 8 on Rf69
#define SENSOR_TIMEOUT_US 24000UL
Ultrasonic sensor(TRIGGER_PIN, ECHO_PIN, SENSOR_TIMEOUT_US);

// used for tracking time passing each loop
unsigned long lastLoopMs = 0;

// time that a jam lasts if no other pulses are received
#define maxJamDurationMs 200UL
unsigned long remainingJamDurationMs = 0;
// target frequency to count as being jammed
#define jamFrequencyHz 1100f
// how long to wait checking for jam pulses
#define jamCheckTimeoutUs 5000UL
// offset from jamFrequencyHz that still counts as being jammed
#define jamFrequencyLeewayHz 10f

// send messages to coordinator for it to forward to gui which treats it as debug info
// because we can't necessarily use serial from a sensor.
void sendDebug(String message) {
  String messageWithRout = "[" + String(ROUT_NUMBER) + "]: " + message;
  uint8_t* cString = messageWithRout.c_str();

  uint8_t buffer[MAX_PACKET_SIZE];
  buffer[0] = DEBUG_PACKET_INDICATOR;
  memcpy(buffer + 1, cString, min(MAX_PACKET_SIZE - 1, strlen(cString)));

  radio.sendToCoordinator(buffer, min(MAX_PACKET_SIZE, strlen(cString) + 1));
}

void handleRadioSetupError(String error) {
  sendDebug(error);
}

// check if pulses are hitting the photodiode. If they are hitting in the
// right frequency, act as though this sensor is "jammed"
void checkIfJammed() {
  // maximum amount of time this function should spend waiting for pulses
  unsigned long timeoutUs = micros() + jamCheckTimeoutUs;

  // if starting in middle of pulse, ignore it so we can catch the
  // rising edge of the next one instead
  while (digitalRead(A1) == HIGH) {
    if (micros() >= timeoutUs) return;
  }

  // wait for first pulse's rising edge
  while (digitalRead(A1) == LOW) {
    if (micros() >= timeoutUs) return;
  }
  unsigned long firstRisingEdge = micros();

  // wait for first pulse to end
  while (digitalRead(A1) == HIGH) {
    if (micros() >= timeoutUs) return;
  }

  // wait for second pulse's rising edge
  while (digitalRead(A1) == LOW) {
    if (micros() >= timeoutUs) return;
  }

  // the period is the time difference between the two rising edges
  unsigned long pulsePeriodUs = micros() - firstRisingEdge;

  // convert the period to a frequency in Hz
  float pulseFrequencyMhz = 1.0 / pulsePeriodUs;
  float pulseFrequencyHz = pulseFrequencyMhz * 1000000;

  // if frequency is close enough to target frequency mark sensor as jammed
  if (abs(pulseFrequencyHz - jamFrequencyHz) <= j
  // how long to wait checking for jam pulsesuencyLeewayHz) {
    remainingJamDurationMs = maxJamDurationMs
    // offset from jamFrequencyHz that still counts as being jammed;
  }
}

// runs at startup
void setup() {
  if (!radio.setup(handleRadioSetupError)) {
    sendDebug("Radio setup failed");
    while (true) {} // block
  }
}

// runs on repeat
void loop() {
  // determine if the photodiode is being hit by the correct frequency
  checkIfJammed();

  // reduce the timer for how long this sensor is jammed
  if (millis() - lastLoopMs > remainingJamDurationMs) {
    remainingJamDurationMs = 0;
  } else {
    remainingJamDurationMs -= millis() - lastLoopMs;
  }
  lastLoopMs = millis();

  // get distance from sensor
  uint16_t data = sensor.read();

  // our ultrasonics don't read higher than 4 meters. They do give results
  // that are >400 when they timeout however (I believe this is the library's fault).
  if (data <= 400 && remainingJamDurationMs <= 0) {
    // build data packet
    uint8_t dataPacket[3 + sizeof(data)];
    dataPacket[0] = SENSOR_DATA_INDICATOR;  // mark as data packet
    dataPacket[1] = DISTANCE_DATA_INDICATOR;  // mark as a distance data packet
    dataPacket[2] = ROUT_NUMBER;  // mark with address of sensor
    memcpy(dataPacket + 3, &data, sizeof(data)); // put data in packet

    // send data packet to sender of data request
    radio.sendToCoordinator(dataPacket, sizeof(dataPacket));
  }

  radio.loop();
}
