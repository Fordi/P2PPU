#ifndef PPUCONFIG_H
#define PPUCONFIG_H
// Config

#define P2PPU_SPRITES 128 // Slots for sprites, can be 1..256.
#define P2PPU_SPRITES_PER_SCAN 32 // Number of sprites allowed per scanline, 1..P2PPU_SPRITES.  consumes 128 bytes of RAM per.
#define P2PPU_BG_WIDTH 32 // BG table size.  32x32 is 2 screens by 2 screens.  Could get away with 17x17 for scrolling.
#define P2PPU_BG_HEIGHT 32

#define P2PPU_WIDTH 128 // Physical size of screen; Pixel 2.0 is 128x128
#define P2PPU_HEIGHT 128

#endif
