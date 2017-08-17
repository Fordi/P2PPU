#ifndef PPU_SSD1351_H
#define PPU_SSD1351_H

#include "PPUDriver.h"

// Configuration

// Pins used for interfacing with the SPI channel the Pixel 2.0's screen is connected to.
#define SSD1351_RST  7
#define SSD1351_DC   8
#define SSD1351_CS   9


#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#if defined (__SAM3X8E__) || (ARDUINO_ARCH_SAMD)
    typedef volatile RwReg PortReg;
    typedef uint32_t PortMask;
#else
    typedef volatile uint8_t PortReg;
    typedef uint8_t PortMask;
#endif


// SSD1351 Commands
// For init
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET   0xA2
#define SSD1351_CMD_SETGPIO         0xB5
#define SSD1351_CMD_FUNCTIONSELECT  0xAB
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_NORMALDISPLAY   0xA6
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_DISPLAYON       0xAF

// For writing to screen
#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C

// Unused in this library
#define SSD1351_CMD_DISPLAYALLON    0xA5
#define SSD1351_CMD_DISPLAYALLOFF   0xA4
#define SSD1351_CMD_INVERTDISPLAY   0xA7
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL  0xBB
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F

// Screen-specific constants
#define SSD1351_WIDTH               0x80
#define SSD1351_HEIGHT              0x80

class PPUDriver_SSD1351 : public PPUDriver {
 public:
  PPUDriver_SSD1351();
  void begin();

  void startData();
  virtual void transmit(uint16_t* data, uint16_t count);
  void endData();
  
  uint8_t getWidth();
  uint8_t getHeight();

private: 
  void writeCommand(uint8_t c);
  void writeData(uint8_t c);
  
  PortReg *chip_select_port, *command_port;
  PortMask chip_select_mask, command_mask;
};

#endif
