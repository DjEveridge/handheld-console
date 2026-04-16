#include "display.h"
#include <Wire.h>

U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void displayInit() {
    Wire.begin();
    u8g2.begin();
}