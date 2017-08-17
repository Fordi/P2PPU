#include "PPUDriver_SSD1351.h"
#include <SPI.h>

PPUDriver_SSD1351::PPUDriver_SSD1351() {
    chip_select_port = portOutputRegister(digitalPinToPort(SSD1351_CS));
    chip_select_mask = digitalPinToBitMask(SSD1351_CS);
    command_port     = portOutputRegister(digitalPinToPort(SSD1351_DC));
    command_mask     = digitalPinToBitMask(SSD1351_DC);
}

/**
 * Write a command to the SSD1351
 **/
void PPUDriver_SSD1351::writeCommand(uint8_t c) {
    *command_port &= ~ command_mask;
    *chip_select_port &= ~ chip_select_mask;
    SPI.transfer(c);
    *chip_select_port |= chip_select_mask;
}
/**
 * Write a single data byte to the SSD1351
 **/
void PPUDriver_SSD1351::writeData(uint8_t c) {
    *command_port |= command_mask;
    *chip_select_port &= ~ chip_select_mask;
    SPI.transfer(c);
    *chip_select_port |= chip_select_mask;  
}

void PPUDriver_SSD1351::startData() {
  //Enable chip
  
  writeCommand(SSD1351_CMD_SETROW);
  writeData(0);
  writeData(SSD1351_HEIGHT - 1);
  
  // Set columns to x..xe inclusive
  writeCommand(SSD1351_CMD_SETCOLUMN);
  writeData(0);
  writeData(SSD1351_WIDTH - 1);
  
  // SPI channel now writes directly to screen
  writeCommand(SSD1351_CMD_WRITERAM);
  // Switch to data mode; screen is now ready to be written
  *chip_select_port &= ~ chip_select_mask;
  *command_port |= command_mask;
}
void PPUDriver_SSD1351::endData() {
  *chip_select_port |= chip_select_mask;
}

/**
 * Initialize the SSD1351
 **/
void PPUDriver_SSD1351::begin(void) {
    // set pin directions
    pinMode(SSD1351_DC, OUTPUT);
    
    // Start SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_MIN_CLOCK_DIVIDER);

    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(SSD1351_CS, OUTPUT);
    digitalWrite(SSD1351_CS, LOW);
    
    pinMode(SSD1351_RST, OUTPUT);
    digitalWrite(SSD1351_RST, HIGH);
    delay(500);
    digitalWrite(SSD1351_RST, LOW);
    delay(500);
    digitalWrite(SSD1351_RST, HIGH);
    delay(500);

    // Initialization Sequence
    // Unlock OLED driver IC MCU interface from entering command (reset)
    writeCommand(SSD1351_CMD_COMMANDLOCK);
    writeData(0x12);
    // Allow access to commands:
    //  DISPLAYOFFSET
    //  PRECHARGE
    //  CLOCKDIV
    //  PRECHARGELEVEL
    //  VCOMH
    //  CONTRASTABC
    writeCommand(SSD1351_CMD_COMMANDLOCK);
    writeData(0xB1);

    // Enter sleep mode
    writeCommand(SSD1351_CMD_DISPLAYOFF);

    // Set clock divider to 0xf (maximum frequency) 0x01 (divide by 2)
    writeCommand(SSD1351_CMD_CLOCKDIV);
    writeCommand(0xF1);
    
    // Set mux ratio to 127
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(127);
    
    // Display orientation mapping
    // 7..6 = 11b (16 bit)
    // 5 = 1b (5/6/5)
    // 4 = 0 (scan is 0 .. MUX(127))
    // 3 is reserved
    // 2 = 1 (color sequence is C/B/A)
    // 1 = 0 (column address 0 is segment 0, default)
    // 0 = 0 (scanlines are horizontal)
    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(0x74);
  
    // Set addressing to whole screen
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(0x00);
    writeData(0x7F);
    
    writeCommand(SSD1351_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);

    // Set the screen's vertical scroll to 0
    writeCommand(SSD1351_CMD_STARTLINE);
    writeData(0);
    writeCommand(SSD1351_CMD_DISPLAYOFFSET);
    writeData(0x0);

    // Disable TFT's onboard GPIO
    writeCommand(SSD1351_CMD_SETGPIO);
    writeData(0x00);
    
    // Enable the internal voltage regulator    
    writeCommand(SSD1351_CMD_FUNCTIONSELECT);
    writeData(0x01);
    
    // Set reset and pre-charge periods
    // 7..4: pre-charge = 3 clk 
    // 3..0: reset = 5 clk
    writeCommand(SSD1351_CMD_PRECHARGE);
    writeCommand(0x32);
 
    // Set COM deselect voltage level
    //  2..0: Vcomh = 0.82 x Vcc
    writeCommand(SSD1351_CMD_VCOMH);
    writeCommand(0x05);

    // Reset to normal display mode
    writeCommand(SSD1351_CMD_NORMALDISPLAY);


    // Set contrast values
    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);

    // Contrast master = 100%
    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(0x0F);

    // Set segment low voltage
    writeCommand(SSD1351_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    
    // Set second pre-charge period
    // 3..0: 1 (1 clk)
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(0x01);
    
    // Exit sleep mode
    writeCommand(SSD1351_CMD_DISPLAYON);
    
    // PPU will always be writing full scanlines
    writeCommand(SSD1351_CMD_SETROW);
    writeData(0);
    writeData(SSD1351_HEIGHT - 1);
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(0);
    writeData(SSD1351_WIDTH - 1);

}


void PPUDriver_SSD1351::transmit(uint16_t* c, uint16_t n) {
  // TODO: Implement blocking DMA if available.
  for (uint16_t i = 0; i < n; i++) {
    SPI.transfer(c[i] >> 8);
    SPI.transfer(c[i] & 0xFF);
  }
}

uint8_t PPUDriver_SSD1351::getWidth() { return SSD1351_WIDTH; }
uint8_t PPUDriver_SSD1351::getHeight() { return SSD1351_HEIGHT; }

