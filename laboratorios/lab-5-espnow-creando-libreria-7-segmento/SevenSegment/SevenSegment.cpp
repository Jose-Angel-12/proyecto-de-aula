#include "SevenSegment.h"

SevenSegment::SevenSegment(const uint8_t segmentPins[8], uint8_t lsbPin, uint8_t msbPin, uint8_t waitTime) {
  memcpy(_segmentPins, segmentPins, sizeof(_segmentPins));
  _lsbPin = lsbPin;
  _msbPin = msbPin;
  _waitTime = waitTime;
}

void SevenSegment::begin() {
  for (int i = 0; i < 8; i++) {
    pinMode(_segmentPins[i], OUTPUT);
    digitalWrite(_segmentPins[i], HIGH);
  }
  pinMode(_lsbPin, OUTPUT);
  pinMode(_msbPin, OUTPUT);
}

void SevenSegment::displayRaw(uint8_t digit) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(_segmentPins[i], segCode[digit][i]);
  }
}

void SevenSegment::displayDigit(uint8_t digit) {
  displayRaw(digit);
}

void SevenSegment::displayNumber(uint8_t number) {
  uint8_t units = number % 10;
  uint8_t tens = number / 10;

  // Mostrar unidades
  displayRaw(16);
  digitalWrite(_lsbPin, HIGH);
  digitalWrite(_msbPin, LOW);
  displayRaw(units);
  delay(_waitTime);

  // Mostrar decenas
  displayRaw(16);
  digitalWrite(_lsbPin, LOW);
  digitalWrite(_msbPin, HIGH);
  displayRaw(tens);
  delay(_waitTime);
}

