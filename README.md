# P2PPU

P2PPU is an example program using [μC PPU](https://github.com/Fordi/UcPPU), written 
for the Pixel 2.0.  It's a simple thing, which renders an opaque background 
layer, two masked background layers, and a sprite layer to a Pixel 2.0.

Please see the documentation for [μC PPU](https://github.com/Fordi/UcPPU).

# Building

First, check μC PPU and its SSD1351 driver out into your Arduino personal libraries
folder.  On linux / OS-X:

    cd ~/Arduino/libraries
    git clone https://github.com/Fordi/UcPPU
    git clone https://github.com/Fordi/UcPPU_SSD1351

On Windows:

    cd "%USERPROFILE%\My Documents"
    git clone https://github.com/Fordi/UcPPU
    git clone https://github.com/Fordi/UcPPU_SSD1351

Then, load up this repository as a sketch in Arduino, and upload it.
