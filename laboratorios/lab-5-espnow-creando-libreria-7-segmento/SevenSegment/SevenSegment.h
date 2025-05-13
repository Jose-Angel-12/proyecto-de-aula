#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <Arduino.h>

class SevenSegment {
  private:
    int *segPins;
    uint8_t LSB_pin, MSB_pin;
    uint8_t segCode[17][8] = {
      {0,0,0,0,0,0,1,1}, {1,0,0,1,1,1,1,1}, {0,0,1,0,0,1,0,1},
      {0,0,0,0,1,1,0,1}, {1,0,0,1,1,0,0,1}, {0,1,0,0,1,0,0,1},
      {0,1,0,0,0,0,0,1}, {0,0,0,1,1,1,1,1}, {0,0,0,0,0,0,0,1},
      {0,0,0,0,1,0,0,1}, {0,0,0,1,0,0,0,1}, {1,1,0,0,0,0,0,1},
      {0,1,1,0,0,0,1,1}, {1,0,0,0,0,1,0,1}, {0,1,1,0,0,0,0,1},
      {0,1,1,1,0,0,0,1}, {1,1,1,1,1,1,1,1}
    };

  public:
    SevenSegment(int *segmentPins, uint8_t LSB, uint8_t MSB);
    void displayDigit(uint8_t digit);
    void displayNumber(uint8_t units, uint8_t tens, uint8_t delayTime);
};

#endif
