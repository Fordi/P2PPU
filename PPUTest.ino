#include "P2PPU.h"
// Pixel 2.0 has a built-in SSD1351 tft, which, 
// coincidentally, is the only driver written as yet.
#include "PPUDriver_SSD1351.h"

// Our boring resource table with a small number font in it.
#include "Resources.h"
#include <SPI.h>

// Create an instance of the TFT driver
PPUDriver_SSD1351 ppuDriver = PPUDriver_SSD1351();
// Create an instance of the PPU
P2PPU ppu = P2PPU(ppuDriver);

void setup(void) {
  // Start the PPU (this will also start the driver and init the TFT)
  ppu.begin();
  // Set the global background color
  ppu.setBackgroundColor(0x0000);
  
  // Enable our sprites to go offscreen by putting the sprite layer's origin at -16, -16
  ppu.setSpriteOffset(16, 16);

  // Populate the background table with tiles
  for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y++) {
    for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x++) {
      uint16_t index = (y * P2PPU_WIDTH + x) % P2PPU_TILES;
      ppu.setBackground(x, y, index, 0);
    }
  }
  // 16x16 correlated sprite
  // 12
  // 34
  ppu.setSprite(0, 0, 1);
  ppu.setSpritePosition(0, 60, 60);
  ppu.setSprite(1, 1, 1);
  ppu.setSpritePosition(0, 68, 60);
  ppu.setSprite(2, 2, 1);
  ppu.setSpritePosition(0, 60, 68);
  ppu.setSprite(3, 3, 1);
  ppu.setSpritePosition(0, 68, 68);
  
}

// Track the background
uint8_t bgX = 63;
uint8_t bgY = 63;
int vX = 3;
int vY = 5;

//Start the sprite centered on the screen: offset + (screen width / 2) - (sprite width / 2)

uint8_t sprX = 72;
uint8_t sprY = 72;
int svX = -2;
int svY = -1;

void loop() {
  //Scroll the background by moving its offset
  bgX += vX;
  bgY += vY;
  if (bgX > 127 || bgX < 0) {
    vX = -vX;
    bgX += vX;
  }
  if (bgY > 127 || bgY < 0) {
    vY = -vY;
    bgY += vY;
  }
  ppu.setBackgroundOffset(bgX, bgY);

  // Move the sprite by shifting its position
  sprX += svX;
  sprY += svY;
  if (sprX > 159 || sprX <= 0) {
    svX = -svX;
    sprX+=svX;
  }
  if (sprY > 159 || sprY <= 0) {
    svY = -svY;
    sprY+=svY;
  }
  ppu.setSpritePosition(0, sprX, sprY);
  ppu.setSpritePosition(1, sprX+8, sprY);
  ppu.setSpritePosition(2, sprX, sprY+8);
  ppu.setSpritePosition(3, sprX+8, sprY+8);
  
  // Finally, render the composition
  ppu.render();
  
}


