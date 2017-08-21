#include <UcPPU.h>
#include <UcPPUDriver.h>
// Pixel 2.0 has a built-in SSD1351 tft, which, 
// coincidentally, is the only driver written as yet.
#include <UcPPU_SSD1351.h>

// Our boring resource table with a small number font in it.
#include "Resources.h"
#include <SPI.h>

// Create an instance of the TFT driver
UcPPU_SSD1351 ppuDriver = UcPPU_SSD1351();
// Create an instance of the PPU
UcPPU ppu = UcPPU(ppuDriver, &tiles[0][0], UCPPU_TILES, &palettes[0][0], UCPPU_PALETTES);

void setup(void) {
  // Start the PPU (this will also start the driver and init the TFT)
  ppu.begin();
  // Set the global background color
  ppu.setBackgroundColor(0x0000);
  uint8_t height = ppuDriver.getHeight();
  uint8_t width = ppuDriver.getWidth();

  ppu.enableLayer(bg0);
  ppu.enableLayer(bg1);
  ppu.enableSprites();
  ppu.enableLayer(fg);
  // Enable our sprites to go offscreen by putting the sprite layer's origin at -16, -16
  ppu.setSpriteOffset(16, 16);

  // Populate the background table with tiles
  for (uint8_t y = 0; y < height >> 2; y+=3) {
    for (uint8_t x = 0; x < width >> 2; x+=3) {
      uint16_t index = (y * width + x) % UCPPU_TILES;
      ppu.setBackground(bg0, x, y, index, index < 10 ? 0 : 2);
    }
  }
  for (uint8_t y = 0; y < height >> 2; y+=5) {
    for (uint8_t x = 0; x < width >> 2; x+=5) {
      uint16_t index = (y * width + x) % UCPPU_TILES;
      ppu.setBackground(bg1, x, y, index, index < 10 ? 1 : 2);
    }
  }
  for (uint8_t y = 0; y < height >> 2; y+=4) {
    for (uint8_t x = 0; x < width >> 2; x+=4) {
      uint16_t index = (y * width + x) % UCPPU_TILES;
      ppu.setBackground(fg, x, y, index, index < 10 ? 0 : 2);
    }
  }
  // 16x16 correlated sprite
  // 12
  // 34
  ppu.setSprite(0, 0, 1, 0, 0);
  ppu.setSprite(1, 1, 1, 0, 1);
  ppu.setSprite(2, 2, 1, 1, 0);
  ppu.setSprite(3, 3, 1, 1, 1);

  
  // Set position for framerate sprites.
  for (uint8_t i = 0; i < 5; i++) {
    ppu.setSpritePosition(i + 4, width - 5 * 8 + i * 8 + 16, 16);
    ppu.setSprite(i + 4, UCPPU_NO_TILE, 1);
  }
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

uint32_t us = 0;
uint32_t frames = 0;

void loop() {
render();  
  
}

void render() {
    //Scroll the background by moving its offset
  uint32_t now = micros();
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
  
  // Correlated sprite movement!
  ppu.setSpritePosition(0, 3, 2, sprX, sprY);
  
  // Finally, render the composition
  ppu.render();
  us += (micros() - now);
  frames++;
  writeFrameTime();
}
void writeFrameTime() {
  uint32_t t = us / (frames * 1000);
  for (uint8_t i = 0; i < 5; i++) {
    // 0123456789 -> 1234567890, since that's how our tiles are laid out.
    uint8_t d = (t) % 10;
    t = t / 10;
    ppu.setSpriteTile(8 - i, d);
  }
}

