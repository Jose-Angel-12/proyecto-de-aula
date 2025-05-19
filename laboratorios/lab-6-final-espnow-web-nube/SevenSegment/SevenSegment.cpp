#include "SevenSegment.h"

SevenSegment::SevenSegment(int *segmentPins, uint8_t LSB, uint8_t MSB) {
  segPins = segmentPins;
  LSB_pin = LSB;
  MSB_pin = MSB;

  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH);
  }

  pinMode(LSB_pin, OUTPUT);
  pinMode(MSB_pin, OUTPUT);
}

void SevenSegment::displayDigit(uint8_t digit) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segPins[i], segCode[digit][i]);
  }
}

void SevenSegment::displayNumber(uint8_t units, uint8_t tens, uint8_t delayTime) {
  displayDigit(16);
  digitalWrite(LSB_pin, HIGH);
  digitalWrite(MSB_pin, LOW);
  displayDigit(units);
  delay(delayTime);

  displayDigit(16);
  digitalWrite(LSB_pin, LOW);
  digitalWrite(MSB_pin, HIGH);
  displayDigit(tens);
  delay(delayTime);
}