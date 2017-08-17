#ifndef P2PPU_H
#define P2PPU_H

#include "PPUConfig.h"
#include "PPUDriver.h"

// Internally-used constants
#define P2PPU_PIXELS P2PPU_WIDTH * P2PPU_HEIGHT
#define P2PPU_NO_TILE 0x7FF // Represents a `null` tile, which will be skipped in rendering
#define P2PPU_NO_PALETTE 0x1F // Represents a `null` palette, which will be skipped in rendering

// Used internally for initialization
#define P2PPU_NONE (P2PPU_NO_TILE << 5) | P2PPU_NO_PALETTE



typedef uint32_t Tile[8];
typedef uint16_t Palette[16];
typedef Tile TileTable[P2PPU_TILES];
typedef Palette PaletteTable[P2PPU_PALETTES];


class P2PPU {
 public:
  P2PPU(PPUDriver& driver/*, uint32_t* tiles, uint32_t* palettes*/);
  /**
   * Call during setup()
   */
  void begin();
  /**
   * Call at drawing phase of loop()
   */
  void render();
  
  /**
   * Modify the PPU
   */
  void setSpriteTile(uint8_t spriteIndex, uint16_t tileIndex);
  void setSpritePalette(uint8_t spriteIndex, uint8_t paletteIndex);
  void setSprite(uint8_t spriteIndex, uint16_t tileIndex, uint8_t paletteIndex);
  void setSprite(uint8_t spriteIndex, uint16_t rawReference);
  void setSpritePosition(uint8_t spriteIndex, uint8_t x, uint8_t y);
  void setSpriteOffset(uint8_t x, uint8_t y);

  void setBackgroundColor(uint16_t color);
  void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
  void setBackgroundOffset(uint8_t x, uint8_t y);
  void setBackgroundTile(uint8_t x, uint8_t y, uint16_t tileIndex);
  void setBackgroundPalette(uint8_t x, uint8_t y, uint8_t paletteIndex);
  void setBackground(uint8_t x, uint8_t y, uint16_t tileIndex, uint8_t paletteIndex);
  void setBackground(uint8_t x, uint8_t y, uint16_t rawReference);

  
  
 private:
  PPUDriver* driver;
  
  uint16_t bg_table[P2PPU_BG_HEIGHT][P2PPU_WIDTH];
  uint8_t bg_offset_x = 0;
  uint8_t bg_offset_y = 0;
  uint8_t spr_offset_x = 0;
  uint8_t spr_offset_y = 0;
  uint16_t sprites[P2PPU_SPRITES][2];
  uint16_t bg_color;
  
  //uint32_t* tiles
  //uint16_t* palettes
};

#endif // SSD1351_H
