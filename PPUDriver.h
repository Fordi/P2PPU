#ifndef PPUDRIVER_H
#define PPUDRIVER_H
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

class PPUDriver {
  public:
    virtual void begin();
    virtual void writeCommand(uint8_t c);
    virtual void writeData(uint8_t c);
    virtual void startData();
    virtual void endData();
    virtual void transmit(uint16_t* data, uint16_t count);
    virtual uint8_t getWidth() = 0;
    virtual uint8_t getHeight() = 0;
};
#endif
