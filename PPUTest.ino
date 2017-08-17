#include "P2PPU.h"
#include "PPUDriver.h"
#include "PPUDriver_SSD1351.h"
#include "Resources.h"
#include <SPI.h>

PPUDriver_SSD1351 ppuDriver = PPUDriver_SSD1351();
P2PPU ppu = P2PPU(ppuDriver);

void setup(void) {
  ppu.begin();
  ppu.setBackgroundColor(0x0000);
  
  // Enable our sprites to go offscreen
  ppu.setSpriteOffset(16, 16);

  for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y++) {
    for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x++) {
      uint16_t index = (y * P2PPU_WIDTH + x) % P2PPU_TILES;
      ppu.setBackground(x, y, index, 0);
    }
  }
  // Sprites 0..3 become a sign
  
  ppu.setSprite(0,  0, 1);
  ppu.setSpritePosition(0, 60, 60);
  ppu.setSprite(1,  1, 1);
  ppu.setSpritePosition(0, 68, 60);
  ppu.setSprite(2,  2, 1);
  ppu.setSpritePosition(0, 60, 68);
  ppu.setSprite(3,  3, 1);
  ppu.setSpritePosition(0, 68, 68);
  
}

uint8_t bgX = 63;
uint8_t bgY = 63;
int vX = 3;
int vY = 5;

//Start centered on screen
uint8_t sprX = 60;
uint8_t sprY = 60;
int svX = -2;
int svY = -1;

void loop() {
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
  
  ppu.render();
  
}


