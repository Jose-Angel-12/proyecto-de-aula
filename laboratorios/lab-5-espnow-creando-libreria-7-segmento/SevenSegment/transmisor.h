#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <Arduino.h>

class SevenSegment {
  public:
    SevenSegment(const uint8_t segmentPins[8], uint8_t lsbPin, uint8_t msbPin, uint8_t waitTime = 5);
    void begin();
    void displayDigit(uint8_t digit);
    void displayNumber(uint8_t number);

  private:
    uint8_t _segmentPins[8];
    uint8_t _lsbPin, _msbPin, _waitTime;

    const uint8_t segCode[17][8] = {
      {0,0,0,0,0,0,1,1}, {1,0,0,1,1,1,1,1}, {0,0,1,0,0,1,0,1},
      {0,0,0,0,1,1,0,1}, {1,0,0,1,1,0,0,1}, {0,1,0,0,1,0,0,1},
      {0,1,0,0,0,0,0,1}, {0,0,0,1,1,1,1,1}, {0,0,0,0,0,0,0,1},
      {0,0,0,0,1,0,0,1}, {0,0,0,1,0,0,0,1}, {1,1,0,0,0,0,0,1},
      {0,1,1,0,0,0,1,1}, {1,0,0,0,0,1,0,1}, {0,1,1,0,0,0,0,1},
      {0,1,1,1,0,0,0,1}, {1,1,1,1,1,1,1,1}
    };

    void displayRaw(uint8_t digit);
};

#endif

