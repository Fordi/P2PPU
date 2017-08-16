#ifndef P2PPU_H
#define P2PPU_H

// Config
#define P2PPU_TILES 280 // Number of tiles you'll define in Resources.h
#define P2PPU_PALETTES 1 // Number of palettes you'll define in Resources.h

#define P2PPU_SPRITES 16 // Slots for sprites, can be 1..256; more is slower
#define P2PPU_BG_WIDTH 32 // BG table size.  32x32 is 2 screens by 2 screens.  Could get away with 17x17 for scrolling.
#define P2PPU_BG_HEIGHT 32


#define P2PPU_WIDTH 128 // Physical size of screen; Pixel 2.0 is 128x128
#define P2PPU_HEIGHT 128


// Internally-used constants
#define P2PPU_PIXELS P2PPU_WIDTH * P2PPU_HEIGHT
#define P2PPU_NO_TILE 0x7FF // Represents a `null` tile, which will be skipped in rendering
#define P2PPU_NO_PALETTE 0x1F // Represents a `null` palette, which will be skipped in rendering

// Used internally for initialization
#define P2PPU_NONE (P2PPU_NO_TILE << 5) | P2PPU_NO_PALETTE

// TODO: Is this needed?
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

typedef uint8_t Tile[8][4];
typedef uint16_t Palette[16];
typedef Tile TileTable[P2PPU_TILES];
typedef Palette PaletteTable[P2PPU_PALETTES];

// Pins used for interfacing with the SPI channel the Pixel 2.0's screen is connected to.
#define P2PPU_RST  7
#define P2PPU_DC   8
#define P2PPU_CS   9

// SSD1351 Commands
// For init
#define P2PPU_CMD_COMMANDLOCK     0xFD
#define P2PPU_CMD_DISPLAYOFF      0xAE
#define P2PPU_CMD_CLOCKDIV        0xB3
#define P2PPU_CMD_MUXRATIO        0xCA
#define P2PPU_CMD_SETREMAP        0xA0
#define P2PPU_CMD_STARTLINE       0xA1
#define P2PPU_CMD_DISPLAYOFFSET   0xA2
#define P2PPU_CMD_SETGPIO         0xB5
#define P2PPU_CMD_FUNCTIONSELECT  0xAB
#define P2PPU_CMD_PRECHARGE       0xB1
#define P2PPU_CMD_VCOMH           0xBE
#define P2PPU_CMD_NORMALDISPLAY   0xA6
#define P2PPU_CMD_CONTRASTABC     0xC1
#define P2PPU_CMD_CONTRASTMASTER  0xC7
#define P2PPU_CMD_SETVSL          0xB4
#define P2PPU_CMD_PRECHARGE2      0xB6
#define P2PPU_CMD_DISPLAYON       0xAF

// For writing to screen
#define P2PPU_CMD_SETCOLUMN       0x15
#define P2PPU_CMD_SETROW          0x75
#define P2PPU_CMD_WRITERAM        0x5C

// Unused in this library
//#define P2PPU_CMD_DISPLAYALLON    0xA5
//#define P2PPU_CMD_DISPLAYALLOFF   0xA4
//#define P2PPU_CMD_INVERTDISPLAY   0xA7
//#define P2PPU_CMD_DISPLAYENHANCE  0xB2
//#define P2PPU_CMD_SETGRAY         0xB8
//#define P2PPU_CMD_USELUT          0xB9
//#define P2PPU_CMD_PRECHARGELEVEL  0xBB
//#define P2PPU_CMD_HORIZSCROLL     0x96
//#define P2PPU_CMD_STOPSCROLL      0x9E
//#define P2PPU_CMD_STARTSCROLL     0x9F



class P2PPU {
 public:
  P2PPU();
  void begin(uint16_t backgroundColor);

  void reset(void);
    
  void render(uint16_t backgroundColor);
  
  void setSpriteTile(uint8_t spriteIndex, uint16_t tileIndex);
  void setSpritePalette(uint8_t spriteIndex, uint8_t paletteIndex);
  void setSprite(uint8_t spriteIndex, uint16_t tileIndex, uint8_t paletteIndex);
  void setSprite(uint8_t spriteIndex, uint16_t rawReference);
  void setSpritePosition(uint8_t spriteIndex, uint8_t x, uint8_t y);
  
  
  void setBackgroundTile(uint8_t x, uint8_t y, uint16_t tileIndex);
  void setBackgroundPalette(uint8_t x, uint8_t y, uint8_t paletteIndex);
  void setBackground(uint8_t x, uint8_t y, uint16_t tileIndex, uint8_t paletteIndex);
  void setBackground(uint8_t x, uint8_t y, uint16_t rawReference);
  
  void setBackgroundOffset(uint8_t x, uint8_t y);
  
  
 private:
  void writeCommand(uint8_t c);
  void writeData(uint8_t c);

  PortReg *chip_select_port, *command_port;
  PortMask chip_select_mask, command_mask;
  uint16_t bg_table[P2PPU_BG_HEIGHT][P2PPU_WIDTH];
  uint8_t bg_offset_x = 0;
  uint8_t bg_offset_y = 0;
  uint16_t sprites[P2PPU_SPRITES][2];
};

#endif // SSD1351_H
