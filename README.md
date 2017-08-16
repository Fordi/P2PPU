# P2PPU

P2PPU is an NES-like Picture Processing Unit for the Pixel 2.0

### Note:

This is a low-priority work in progress, and I'm aware it's broken for the 
moment (specifically, the tiles you ask for are not the tiles you get).  Any 
pull requests you can offer will be looked at immediately.

## Breakdown

The NES' PPU was laid out simply:

    * In ROM:
        * Pattern tables, containing 8x8 sprite image data
        * Palettes, containing the color data to be applied to the patterns
    * In PPU RAM
        * A pair of name tables, indicating which pattern/palettes go where, 
            aligned to 8x8 boundaries

        * A sprite table, specifying the positions of the sprites, their 
            palettes, and their patterns

This library implements a similar scheme.  To make use of it, you need to first 
set up Resources.h:

`tiles` should be an array of `P2PPU_TILES` `Tile` structures, where a `Tile` 
is an 8x4 array of bytes, each byte representing a big-endian pixel pair.

`palettes` should be an array of `P2PPU_PALETTES` `Palette` structures, where a
`Palette` is a 16-element array of 16-bit words, representing rgb565 colors.

Once you have that ready, you can run this boring sample program.

    #include "P2PPU.h"
    #include "Resources.h"

    P2PPU ppu = P2PPU();

    void setup(void) {
      ppu.begin(0x0000);
      for (uint8_t y = 0; y < P2PPU_HEIGHT >> 2; y++) {
        for (uint8_t x = 0; x < P2PPU_WIDTH >> 2; x++) {
          uint16_t index = (y * (P2PPU_HEIGHT >> 2) + x) % P2PPU_TILES;
          ppu.setBackground(x, y, index, 0);
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

All it does is fill the screen with tiles, set up a sprite then scroll both
around.  Fun stuff!



