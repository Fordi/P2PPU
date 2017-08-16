# P2PPU

P2PPU is an NES-like Picture Processing Unit for the [Pixel 2.0](](https://www.kickstarter.com/projects/rabidprototypes/pixel-20-the-arduino-compatible-smart-display)), which is a neat little Cortex M0 / SSD1351 based Arduino-compatible board with OLED screen.

### Note:

This is a low-priority work in progress, and I'm aware it's broken for the 
moment (see [open issues](https://github.com/Fordi/P2PPU/issues)).  
`[Bounty]`-tagged issues aren't meant to imply a reward, just that they're 
free for anyone to fix.  Tickets for which I want to reserve responsibility
will be marked `[Hands-off]`.  That said, all pull requests you can offer 
will be looked at immediately, regardless of the ticket's tag.

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

There's a sample Resources.h in the repository; use that as your guide for 
resource preparation.

Once you have that ready, you can run the boring sample program, `PPUTest.ino`.



All it does is fill the screen with tiles, set up a sprite then scroll both
around.  Fun stuff!



