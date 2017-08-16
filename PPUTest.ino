#include "P2PPU.h"
#include "Resources.h"

P2PPU ppu = P2PPU();

void setup(void) {
  ppu.begin(0x0000);
  for (uint8_t y = 0; y < P2PPU_BG_HEIGHT; y++) {
    for (uint8_t x = 0; x < P2PPU_BG_WIDTH; x++) {
      ppu.setBackground(x, y, testScreen[y][x]);
    }
  }
}
uint8_t bgX = 0;
uint8_t bgY = 63;
int vX = 1;
int vY = 1;
void loop() {
  ppu.render(0x0000);
  bgX += vX;
  bgY += vY;
  if (bgX >= 127 || bgX <= 0) {
    vX = -vX;
  }
  if (bgY >= 127 || bgY <= 0) {
    vY = -vY;
  }
  ppu.setBackgroundOffset(bgX, bgY);
}


