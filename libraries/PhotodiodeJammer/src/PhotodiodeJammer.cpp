#include "PhotodiodeJammer.h"

PhotodiodeJammer::PhotodiodeJammer(
  uint8_t photodiodePin,
  float targetFrequency
):
  photodiodePin(photodiodePin),
  targetFrequencyHz(targetFrequency)
{}

void PhotodiodeJammer::setup() {
  pinMode(photodiodePin, INPUT);
}

void PhotodiodeJammer::loop() {
  if (isJammed()) {
    jamDurationMs = maxJamDurationMs;
    Serial.println("jammed");
  }

  unsigned long timeSinceLastLoopMs = millis() - lastLoopTimestampMs;
  lastLoopTimestampMs = millis();
  if (timeSinceLastLoopMs > jamDurationMs) {
    jamDurationMs = 0;
  } else {
    jamDurationMs -= timeSinceLastLoopMs;
  }
}

static void PhotodiodeJammer::pipeJammablePacket(
  const uint8_t* packet,
  size_t packetSize,
  void* handlerContext
) {
  static_cast<PhotodiodeJammer*>(handlerContext)->pipeJammablePacket(packet, packetSize);
}
void PhotodiodeJammer::pipeJammablePacket(
  const uint8_t* packet,
  size_t packetSize
) {
  if (jamDurationMs > 0) return;

  packetHandler(packet, packetSize, packetHandlerContext);
}

void PhotodiodeJammer::setUnjammedPacketHandler(
  void (*newPacketHandler)(const uint8_t*, size_t, void*),
  void* context = nullptr
) {
  packetHandler = newPacketHandler;
  packetHandlerContext = context;
}

bool PhotodiodeJammer::isJammed() {
  // maximum amount of time this function should spend waiting for pulses
  unsigned long timeoutUs = micros() + jamCheckTimeoutUs;

  // if starting in middle of pulse, ignore it so we can catch the
  // rising edge of the next one instead
  while (digitalRead(photodiodePin) == HIGH) {
    if (micros() >= timeoutUs) return false;
  }

  // wait for first pulse's rising edge
  while (digitalRead(photodiodePin) == LOW) {
    if (micros() >= timeoutUs) return false;
  }
  unsigned long firstRisingEdge = micros();

  // wait for first pulse to end
  while (digitalRead(photodiodePin) == HIGH) {
    if (micros() >= timeoutUs) return false;
  }

  // wait for second pulse's rising edge
  while (digitalRead(photodiodePin) == LOW) {
    if (micros() >= timeoutUs) return false;
  }

  // the period is the time difference between the two rising edges
  unsigned long pulsePeriodUs = micros() - firstRisingEdge;

  // convert the period to a frequency in Hz
  float pulseFrequencyMhz = 1.0 / pulsePeriodUs;
  float pulseFrequencyHz = pulseFrequencyMhz * 1000000;

  // if frequency is close enough to target frequency mark sensor as jammed
  return abs(pulseFrequencyHz - targetFrequencyHz) <= jamFrequencyLeewayHz;
}