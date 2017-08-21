# P2PPU

P2PPU is an example program using [μC PPU](https://github.com/Fordi/UcPPU), written 
for the Pixel 2.0.  It's a simple thing, which renders an opaque background 
layer, two masked background layers, and a sprite layer to a Pixel 2.0.

Please see the documentation for [μC PPU](https://github.com/Fordi/UcPPU).

# Demo

For the impatient, there's a [video](https://youtu.be/1ZhZSGqO0Ak) of this demo code in action.

# Building

First, check μC PPU and its [display drivers](https://github.com/Fordi/UcPPU_DisplayDrivers) out into your Arduino personal libraries
folder.  On linux / OS-X:

    cd ~/Arduino/libraries
    git clone https://github.com/Fordi/UcPPU
    git clone https://github.com/Fordi/UcPPU_DisplayDrivers

On Windows:

    cd "%USERPROFILE%\My Documents"
    git clone https://github.com/Fordi/UcPPU
    git clone https://github.com/Fordi/UcPPU_DisplayDrivers

Then, load up this repository as a sketch in Arduino, and upload it.
