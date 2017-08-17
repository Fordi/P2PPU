/************************************************************************
 * Hardware-hugging NES-style Picture Processing Unit for the Pixel 2.0 *
 ************************************************************************/

#include "P2PPU.h"
#include "PPUConfig.h"
#include "Resources.h"
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>


void P2PPU::render() {
    // Allocate enough space for one {spriteIndex} per-line
    // The structure of this is scanlineSprite[y][i], where `i` will grow with more sprites on the same scanlines.
    // Similar limit to the SNES of 32 sprites per scanline
    uint8_t scanSprites[P2PPU_HEIGHT][P2PPU_SPRITES_PER_SCAN];
    uint8_t ssi;
    uint32_t pixelLine;
    uint16_t x, y;
    uint8_t index;
    int16_t tile_y, tile_x;
    uint16_t tile;
    uint16_t scanline[P2PPU_WIDTH];
    int16_t table_y, table_x;
    uint8_t palette;
    uint8_t pi;
    uint16_t entry;
    int16_t screen_x, screen_y;
    uint16_t color;
    uint32_t mask;
    uint32_t shift;
    memset(&scanSprites, 0xFF, P2PPU_HEIGHT * P2PPU_SPRITES_PER_SCAN);
    
    
    // Run over each sprite
    
    for (index = 0; index < P2PPU_SPRITES; index++) {
        // Pull the palette/tile entry
        entry = sprites[index][0];
        
        // No tile; skip this sprite
        if ((entry & 0x7FF) == P2PPU_NO_TILE) { 
          continue; 
        }
        entry = sprites[index][1];
        tile_x = (entry >> 8) - spr_offset_x;
        if (tile_x + 7 < 0 || tile_x >= P2PPU_WIDTH) { continue; }
        tile_y = (entry & 0xFF) - spr_offset_y;
        if (tile_y + 7 < 0 || tile_y >= P2PPU_WIDTH) { continue; }
        
        for (y = 0, screen_y = tile_y; y < 8; y++, screen_y++) {
            if (screen_y < 0) { continue; }
            if (screen_y >= P2PPU_HEIGHT) { break; }
            // Find the next empty slot
            for (ssi = 0; ssi < P2PPU_SPRITES_PER_SCAN; ssi++) {
              if (scanSprites[screen_y][ssi] == 0xFF) { break; }
            }
            // Long as we didn't walk past the end of the buffer, add the sprite reference
            if (ssi < P2PPU_SPRITES_PER_SCAN) {
              scanSprites[screen_y][ssi] = index;
            }
        }
    }
    
    
    
    // Tell the screen it's going to be recieving data
    driver->startData();
    
    for (y = bg_offset_y, screen_y = 0; screen_y < P2PPU_HEIGHT; y++, screen_y++) {
        //Initialize the scanline
        memset(scanline, bg_color, P2PPU_WIDTH << 1);

        // TODO: function renderNametableScanline(scanline, nameTable, y) {
        
        // Write the background
        // Y Index into the background table
        table_y = y >> 3;
        // Y pixel within the tile
        tile_y = y & 7;
        for (x = bg_offset_x, screen_x = 0; screen_x <= P2PPU_WIDTH; x++, screen_x++) {
            // Draw the background tiles
            // X index into the background table
            table_x = x >> 3;
            // X pixel within the tile (0..7)
            tile_x = x & 7;
            // Get the background table entry
            entry = bg_table[table_y][table_x];
            // Extract the tile index
            tile = entry & 0x7FF;
            if (tile != P2PPU_NO_TILE) {
              //Extract the palette index
              palette = (entry >> 11) & 0x1F;
              // Get the line of pixels
              pixelLine = tiles[tile][tile_y];
              shift = 32 - (tile_x << 2);
              mask = 0xF << shift;
              pi = ((pixelLine & mask) >> shift) & 0xF;
              if (palette != P2PPU_NO_PALETTE) {
                scanline[screen_x] = palettes[palette][pi];
              } else {
                pi = pi << 1;
                scanline[screen_x] = (pi << 11) | (pi << 6) | pi;
              }
            }
        }
        
        // endFunction
        
        // TODO: if (layerFlags & P2PPU_LAYER_BG0 != 0) 
        // TODO:  renderNametableScanline(&scanline, &bg_table0, y) 
        
        // TODO: if (layerFlags & P2PPU_LAYER_BG1 != 0) 
        // TODO:  renderNametableScanline(&scanline, &bg_table1, y)
        
        // TODO: if (layerFlags & P2PPU_LAYER_SPR != 0) 
        // Scan over `scanSprites`
        
        // Should this be encapsulated privately?  It'd need a ref to 
        //  scanSprites...
        
        for (ssi = 0; ssi < P2PPU_SPRITES_PER_SCAN; ssi++) {
          index = scanSprites[screen_y][ssi];
          
          if (index == 0xFF) { break; }
          //Grab the sprite's tile/palette reference
          entry = sprites[index][0];
          // grab the tile; tile and palette can not at this point be NO_TILE 
          // or NO_PALETTE
          tile = entry & 0x7FF;
          palette = (entry >> 11) & 0x1F;
          
          // Get the sprite position
          entry = sprites[index][1];
          
          // sprite position
          table_x = (entry >> 8) - spr_offset_x;
          table_y = (entry & 0xFF) - spr_offset_y;
          // Position of current scanline on the screen
          tile_y = screen_y - table_y;
          
          pixelLine = tiles[tile][tile_y];
          
          for (x = 0, screen_x = table_x; x < 8; x++, screen_x++) {
            if (screen_x < 0) { continue; }
            if (screen_x >= P2PPU_WIDTH) { break; }
            
            shift = 32 - (x << 2);
            mask = 0xF << shift;
            pi = ((pixelLine & mask) >> shift) & 0xF;
            if (pi) {
              if (palette != P2PPU_NO_PALETTE) {
                scanline[screen_x] = palettes[palette][pi];
              } else {
                pi = (pi << 1) & 0x1f;
                scanline[screen_x] = (pi << 11) | (pi << 6) | pi;
              }
            }
          }
          
        }
        //memset(scanline, 0x5A, P2PPU_WIDTH << 1);
        
        // TODO: if (layerFlags & P2PPU_LAYER_FG != 0) 
        // TODO:   renderNametableScanline(&scanline, &fg_table, y) 
        
        // Flush the scanline
        driver->transmit(scanline, P2PPU_WIDTH);
    }
    // Close the screen
    driver->endData();
}

void P2PPU::setSpriteTile(uint8_t spriteIndex, uint16_t tileIndex) {
  sprites[spriteIndex][0] = (sprites[spriteIndex][0] & 0xF800) | (tileIndex & 0x7FF);
}
void P2PPU::setSpritePalette(uint8_t spriteIndex, uint8_t paletteIndex) {
  sprites[spriteIndex][0] = (sprites[spriteIndex][0] & 0x7FF) | ((paletteIndex & 0x1F) << 11);
}
void P2PPU::setSprite(uint8_t spriteIndex, uint16_t tileIndex, uint8_t paletteIndex) {
  sprites[spriteIndex][0] = (tileIndex & 0x7FF) | ((paletteIndex & 0x1F) << 11);
}
void P2PPU::setSprite(uint8_t spriteIndex, uint16_t rawReference) {
  sprites[spriteIndex][0] = rawReference;
}

void P2PPU::setSpritePosition(uint8_t spriteIndex, uint8_t x, uint8_t y) {
  sprites[spriteIndex][1] = (x << 8) | y;
}

void P2PPU::setBackgroundTile(uint8_t x, uint8_t y, uint16_t tileIndex) {
  bg_table[y][x] = (bg_table[y][x] & 0xF800) | (tileIndex & 0x7FF);
}
void P2PPU::setBackgroundPalette(uint8_t x, uint8_t y, uint8_t paletteIndex) {
  bg_table[y][x] = (bg_table[y][x] & 0x7FF) | ((paletteIndex & 0x1F) << 11);
}
void P2PPU::setBackground(uint8_t x, uint8_t y, uint16_t tileIndex, uint8_t paletteIndex) {
  bg_table[y][x] = (tileIndex & 0x7FF) | ((paletteIndex & 0x1F) << 11);
}
void P2PPU::setBackground(uint8_t x, uint8_t y, uint16_t rawReference) {
  bg_table[y][x] = rawReference;
}
void P2PPU::setBackgroundOffset(uint8_t x, uint8_t y) {
  bg_offset_x = x;
  bg_offset_y = y;
}

void P2PPU::setSpriteOffset(uint8_t x, uint8_t y) {
  spr_offset_x = x;
  spr_offset_y = y;
}


void P2PPU::setBackgroundColor(uint16_t color) {
  bg_color = color;
}
void P2PPU::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
  bg_color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

P2PPU::P2PPU(PPUDriver& ppuDriver) {
    driver = &ppuDriver;
    for (uint8_t y = 0; y < 32; y++) {
      for (uint8_t x = 0; x < 32; x++) {
        bg_table[y][x] = P2PPU_NONE;
      }
    }
    for (uint8_t index = 0; index < P2PPU_SPRITES; index++) {
      sprites[index][0] = P2PPU_NONE;
      sprites[index][1] = 0x0000;
    }
    bg_offset_x = 0;
    bg_offset_y = 0;
    spr_offset_x = 0;
    spr_offset_y = 0;
    bg_color = 0x0000;
}
void P2PPU::begin() {
  driver->begin();
}


