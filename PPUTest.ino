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
P2PPU ppu = P2PPU(ppuDriver, &tiles[0][0], P2PPU_TILES, &palettes[0][0], P2PPU_PALETTES);

void setup(void) {
  // Start the PPU (this will also start the driver and init the TFT)
  ppu.begin();
  // Set the global background color
  ppu.setBackgroundColor(0x0000);


  ppu.enableLayer(bg0);
  ppu.enableLayer(bg1);
  ppu.enableSprites();
  ppu.enableLayer(fg);
  // Enable our sprites to go offscreen by putting the sprite layer's origin at -16, -16
  ppu.setSpriteOffset(16, 16);

  // Populate the background table with tiles
  for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y+=3) {
    for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x+=3) {
      uint16_t index = (y * P2PPU_WIDTH + x) % P2PPU_TILES;
      ppu.setBackground(bg0, x, y, index, 0);
    }
  }
  for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y+=5) {
    for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x+=5) {
      uint16_t index = (y * P2PPU_WIDTH + x) % P2PPU_TILES;
      ppu.setBackground(bg1, x, y, index, 1);
    }
  }
  for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y+=4) {
    for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x+=4) {
      uint16_t index = (y * P2PPU_WIDTH + x) % P2PPU_TILES;
      ppu.setBackground(fg, x, y, index, 0);
    }
  }
  // 16x16 correlated sprite
  // 12
  // 34
  ppu.setSprite(0, 0, 1, 0, 0);
  ppu.setSprite(1, 1, 1, 0, 1);
  ppu.setSprite(2, 2, 1, 1, 0);
  ppu.setSprite(3, 3, 1, 1, 1);
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
render();  
  
}

void render() {
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
  ppu.setBackgroundOffset(bg0, bgX, bgY);
  ppu.setBackgroundOffset(bg1, 127-bgX, 127-bgY);
  ppu.setBackgroundOffset(fg, bgY, bgX);

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

