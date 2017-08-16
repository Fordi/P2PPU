# P2PPU

P2PPU is an NES-like Picture Processing Unit for the [Pixel 2.0](https://www.kickstarter.com/projects/rabidprototypes/pixel-20-the-arduino-compatible-smart-display), which is a neat little Cortex M0 based Arduino-compatible board with OLED SSD1351 (16-bit, 128x128) screen.

### Note:

This is a low-priority work in progress, and I'm aware it's broken for the 
moment (see [open issues](https://github.com/Fordi/P2PPU/issues)).  
`[Free-for-all]`-tagged issues are free for anyone to fix (though I'll probably 
get to them).  Tickets for which I want to reserve responsibility
will be marked `[Hands-off]`.  That said, all pull requests you can offer 
will be looked at immediately, regardless of the ticket's tag.

## Motivation

When I got my Pixel 2.0 in the mail, I was _super_ excited.  I was gonna totes
write a game specific to this device, using all the framebuffer-fu I'd learned
from various projects I've worked on over the years, starting with some VGA 
programming in the 90's.

Sadly, framebuffer-style deployment of pixel data through the narrow little SPI 
channel proved problematic - at first getting me a mere ~6 FPS using 
Adafruit's SSD1351 library to just throw rectangles at the screen.

With some tweaks to the SSD1351 lib, ultimately breaking its cross-compatibility,
I was able to get a full-screen framerate of around 9.5 FPS - that's fast enough
for some jerky gaming action!

But there's another problem: ain't no way you can fit a 128x128x16 bit framebuffer
in the Pixel's ram; that's 32k and it's only _got_ 32k.  So, I figured I'd look at 
how memory-constrained systems solved it in the past.  I knew peripherally about 
the NES' PPU, and thought that'd be a good place to start.

## Breakdown

Note: this isn't detailed or accurate, just a discussion on how the 
original device worked at a high-level.

The NES' PPU was laid out simply:

    * In ROM:
        * Pattern tables, containing 8x8 sprite image data
        * Palettes, containing the color data to be applied to the patterns
    * In PPU RAM
        * A pair of name tables, indicating which pattern/palettes go where, 
            aligned to 8x8 boundaries

        * A sprite table, specifying the positions of the sprites, their 
            palettes, and their patterns

The NES did rendering on a per-scanline basis, which meant that it could skip
a lot of the memory needs required of a full framebuffer.

This library implements a similar scheme.  To make use of it, you need to first 
set up Resources.h:

`tiles` should be an array of `P2PPU_TILES` `Tile` structures, where a `Tile` 
is an 8x4 array of bytes, each byte representing a big-endian pixel pair.

`palettes` should be an array of `P2PPU_PALETTES` `Palette` structures, where a
`Palette` is a 16-element array of 16-bit words, representing rgb565 colors.

There's a sample Resources.h in the repository; use that as your guide for 
resource preparation.

Once you have that ready, you can run the boring sample program, `PPUTest.ino`.  
All it does is fill the screen with tiles, set up a sprite then scroll both
around.  Fun stuff!

Using the `ppu` local, you can place and palette background tiles and sprites, 
set the background's offset, etc.  Once your code is happy with its composition, 
it can call ppu.render() to throw it at the screen.


