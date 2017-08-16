/************************************************************************
 * Hardware-hugging NES-style Picture Processing Unit for the Pixel 2.0 *
 ************************************************************************/

#include "P2PPU.h"
#include "Resources.h"
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

/**
 * Write a command to the SSD1351
 **/
void P2PPU::writeCommand(uint8_t c) {
    *command_port &= ~ command_mask;
    *chip_select_port &= ~ chip_select_mask;
    SPI.transfer(c);
    *chip_select_port |= chip_select_mask;
}
/**
 * Write a single data byte to the SSD1351
 **/
void P2PPU::writeData(uint8_t c) {
    *command_port |= command_mask;
    *chip_select_port &= ~ chip_select_mask;
    SPI.transfer(c);
    *chip_select_port |= chip_select_mask;  
}

/**
 * Initialize the SSD1351
 **/
void P2PPU::begin(uint16_t color) {
    // set pin directions
    pinMode(P2PPU_DC, OUTPUT);
    
    // Start SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_MIN_CLOCK_DIVIDER);

    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(P2PPU_CS, OUTPUT);
    digitalWrite(P2PPU_CS, LOW);
    
    pinMode(P2PPU_RST, OUTPUT);
    digitalWrite(P2PPU_RST, HIGH);
    delay(500);
    digitalWrite(P2PPU_RST, LOW);
    delay(500);
    digitalWrite(P2PPU_RST, HIGH);
    delay(500);

    // Initialization Sequence
    // Unlock OLED driver IC MCU interface from entering command (reset)
    writeCommand(P2PPU_CMD_COMMANDLOCK);
    writeData(0x12);
    // Allow access to commands:
    //  DISPLAYOFFSET
    //  PRECHARGE
    //  CLOCKDIV
    //  PRECHARGELEVEL
    //  VCOMH
    //  CONTRASTABC
    writeCommand(P2PPU_CMD_COMMANDLOCK);
    writeData(0xB1);

    // Enter sleep mode
    writeCommand(P2PPU_CMD_DISPLAYOFF);

    // Set clock divider to 0xf (maximum frequency) 0x01 (divide by 2)
    writeCommand(P2PPU_CMD_CLOCKDIV);
    writeCommand(0xF1);
    
    // Set mux ratio to 127
    writeCommand(P2PPU_CMD_MUXRATIO);
    writeData(127);
    
    // Display orientation mapping
    // 7..6 = 11b (16 bit)
    // 5 = 1b (5/6/5)
    // 4 = 0 (scan is 0 .. MUX(127))
    // 3 is reserved
    // 2 = 1 (color sequence is C/B/A)
    // 1 = 0 (column address 0 is segment 0, default)
    // 0 = 0 (scanlines are horizontal)
    writeCommand(P2PPU_CMD_SETREMAP);
    writeData(0x74);
  
    // Set addressing to whole screen
    writeCommand(P2PPU_CMD_SETCOLUMN);
    writeData(0x00);
    writeData(0x7F);
    
    writeCommand(P2PPU_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);

    // Set the screen's vertical scroll to 0
    writeCommand(P2PPU_CMD_STARTLINE);
    writeData(0);
    writeCommand(P2PPU_CMD_DISPLAYOFFSET);
    writeData(0x0);

    // Disable TFT's onboard GPIO
    writeCommand(P2PPU_CMD_SETGPIO);
    writeData(0x00);
    
    // Enable the internal voltage regulator    
    writeCommand(P2PPU_CMD_FUNCTIONSELECT);
    writeData(0x01);
    
    // Set reset and pre-charge periods
    // 7..4: pre-charge = 3 clk 
    // 3..0: reset = 5 clk
    writeCommand(P2PPU_CMD_PRECHARGE);
    writeCommand(0x32);
 
    // Set COM deselect voltage level
    //  2..0: Vcomh = 0.82 x Vcc
    writeCommand(P2PPU_CMD_VCOMH);
    writeCommand(0x05);

    // Reset to normal display mode
    writeCommand(P2PPU_CMD_NORMALDISPLAY);


    // Set contrast values
    writeCommand(P2PPU_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);

    // Contrast master = 100%
    writeCommand(P2PPU_CMD_CONTRASTMASTER);
    writeData(0x0F);

    // Set segment low voltage
    writeCommand(P2PPU_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    
    // Set second pre-charge period
    // 3..0: 1 (1 clk)
    writeCommand(P2PPU_CMD_PRECHARGE2);
    writeData(0x01);
    
    // Exit sleep mode
    writeCommand(P2PPU_CMD_DISPLAYON);
    
    // PPU will always be writing full scanlines
    writeCommand(P2PPU_CMD_SETROW);
    writeData(0);
    writeData(P2PPU_HEIGHT - 1);
    writeCommand(P2PPU_CMD_SETCOLUMN);
    writeData(0);
    writeData(P2PPU_WIDTH - 1);

    //Clear the screen
    writeCommand(P2PPU_CMD_WRITERAM);
    *command_port |= command_mask;
    *chip_select_port &= ~ chip_select_mask;
    
    for (uint16_t i = 0; i < P2PPU_PIXELS; i++) {
      SPI.transfer(color >> 8);
      SPI.transfer(color & 0xFF);
    }
    *chip_select_port |= chip_select_mask;  
}

void P2PPU::render(uint16_t background) {
    // function requires (at least) 266 bytes RAM
    uint16_t scanline[P2PPU_WIDTH];
    uint16_t x, y;
    uint8_t table_y, table_x;
    uint8_t tile_y, tile_x;
    uint16_t tile;
    uint8_t palette;
    uint8_t index;
    uint8_t pi;
    uint16_t entry;
    // select chip
    *chip_select_port &= ~ chip_select_mask;
    
    // Send commands to initialize full-screen write
    *command_port &= ~ command_mask;    
    SPI.transfer(P2PPU_CMD_SETROW);
    *command_port |= command_mask;
    SPI.transfer(0);
    SPI.transfer(P2PPU_HEIGHT - 1);
    *command_port &= ~ command_mask;
    SPI.transfer(P2PPU_CMD_SETCOLUMN);
    *command_port |= command_mask;
    SPI.transfer(0);
    SPI.transfer(P2PPU_WIDTH - 1);
    *command_port &= ~ command_mask;
    SPI.transfer(P2PPU_CMD_WRITERAM);
    
    // Gonna write some data...
    *command_port |= command_mask;
    for (y = bg_offset_y; y <= bg_offset_y + P2PPU_HEIGHT - 1; y++) {
        //Initialize the scanline
        memset(scanline, background, P2PPU_WIDTH << 1);
        // Write the background
        table_y = y >> 3;
        tile_y = y & 7;
        for (x = bg_offset_x; x <= bg_offset_x + P2PPU_WIDTH - 1; x++) {
            // Draw the background tiles
            tile_x = x & 7;
            table_x = x >> 3;
            entry = bg_table[table_y][table_x];
            tile = (entry & 0xFFE0) >> 5;
            
            if (tile != P2PPU_NO_TILE) {
              palette = entry & P2PPU_NO_PALETTE;
              if (palette != P2PPU_NO_PALETTE) {
                if (tile_x & 1) {
                    pi = (tiles[tile][tile_y][tile_x >> 1] & 0xF0) >> 4;
                } else {
                    pi = tiles[tile][tile_y][tile_x >> 1] & 0xF;
                }
                scanline[x - bg_offset_x] = palettes[palette][pi];
              }
            }
        }
        // Gotta be a better way than to loop this every scanline...
        for (index = 0; index < P2PPU_SPRITES; index++) {
            table_y = sprites[index][1] & 0xFF;
            // Sprite is on this line vertically
            if (table_y > y && table_y < y + 8) {
                table_x = sprites[index][1] >> 8;
                // Sprite is on-screen horizontally
                if (table_x > bg_offset_x && table_x < P2PPU_WIDTH + bg_offset_x) {
                    tile_y = table_y - y;
                    for (tile_x = 0; tile_x < 8; tile_x++) {
                        entry = sprites[index][0];
                        tile = (entry & 0xFFE0) >> 5;
                        if (tile != P2PPU_NO_TILE) {
                          palette = entry & P2PPU_NO_PALETTE;
                          if (palette != P2PPU_NO_PALETTE) {
                            if (tile_x & 1) {
                                pi = (tiles[tile][tile_y][tile_x >> 1] & 0xF0) >> 4;
                            } else {
                                pi = tiles[tile][tile_y][tile_x >> 1] & 0xF;
                            }
                            // For sprites, palette index 0 is transparent.
                            if (pi) {
                                scanline[table_x + tile_x - bg_offset_x] = palettes[palette][pi];
                            }
                          }
                        }
                    }
                }
            }
        }
        // Flush the scanline
        for (x = 0; x <= P2PPU_WIDTH - 1; x++) {
            SPI.transfer(scanline[x] >> 8);
            SPI.transfer(scanline[x] & 0xFF);
        }
        
    }
    // Deselect the chip
    *chip_select_port |= chip_select_mask;
}

void P2PPU::setSpriteTile(uint8_t spriteIndex, uint16_t tileIndex) {
  sprites[spriteIndex][0] = (sprites[spriteIndex][0] & 0x1f) | (tileIndex << 5);
}
void P2PPU::setSpritePalette(uint8_t spriteIndex, uint8_t paletteIndex) {
  sprites[spriteIndex][0] = (sprites[spriteIndex][0] & 0xFFE0) | paletteIndex;
}
void P2PPU::setSpritePosition(uint8_t spriteIndex, uint8_t x, uint8_t y) {
  sprites[spriteIndex][1] = x << 8 | y;
}
void P2PPU::setSprite(uint8_t spriteIndex, uint16_t tileIndex, uint8_t paletteIndex) {
  sprites[spriteIndex][0] = (tileIndex << 5) | paletteIndex;
}
void P2PPU::setSprite(uint8_t spriteIndex, uint16_t rawReference) {
  sprites[spriteIndex][0] = rawReference;
}

void P2PPU::setBackgroundTile(uint8_t x, uint8_t y, uint16_t tileIndex) {
  bg_table[y][x] = (bg_table[y][x] & 0x1F) | (tileIndex << 5);
}
void P2PPU::setBackgroundPalette(uint8_t x, uint8_t y, uint8_t paletteIndex) {
  bg_table[y][x] = (bg_table[y][x] & 0xFFE0) | paletteIndex;
}
void P2PPU::setBackground(uint8_t x, uint8_t y, uint16_t tileIndex, uint8_t paletteIndex) {
  bg_table[y][x] = (tileIndex << 5) | paletteIndex;
}
void P2PPU::setBackground(uint8_t x, uint8_t y, uint16_t rawReference) {
  bg_table[y][x] = rawReference;
}
void P2PPU::setBackgroundOffset(uint8_t x, uint8_t y) {
  bg_offset_x = x;
  bg_offset_y = y;
}


P2PPU::P2PPU() {
    chip_select_port      = portOutputRegister(digitalPinToPort(P2PPU_CS));
    chip_select_mask   = digitalPinToBitMask(P2PPU_CS);
    command_port      = portOutputRegister(digitalPinToPort(P2PPU_DC));
    command_mask   = digitalPinToBitMask(P2PPU_DC);
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
}
