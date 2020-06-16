/**
 * 
 * 
 * Download model here: 
 *     http://dl.sipeed.com/MAIX/MaixPy/model/mobilenet_0x300000.kfpkg
 * Unpack it(zip format), get m.kmodel, change name to a short name "m" for example,
 * put it in SD card at root path
 * 
 * 
 * 
 */


#include <Arduino.h>
#include <Maixduino_OV7740.h>
#include <Sipeed_ST7789.h>
#include <Wire.h>
#include "MBNet_1000.h"
#include "Maix_KPU.h"

#define AXP173_ADDR 0x34

SPIClass spi_(SPI0); // MUST be SPI0 for Maix series on board LCD
Sipeed_ST7789 lcd(240, 240, spi_);
Maixduino_OV7740 camera(224, 224, PIXFORMAT_YUV422);
KPUClass KPU;
MBNet_1000 mbnet(KPU, lcd, camera);

const char* kmodel_name = "m";


void axp173_init()
{
    Wire.begin((uint8_t) SDA, (uint8_t) SCL, 400000);
    Wire.beginTransmission(AXP173_ADDR);
    int err = Wire.endTransmission();
    if (err)
    {
        Serial.printf("Power management ic not found.\n");
        return;
    }
    Serial.printf("AXP173 found.\n");
    // Clear the interrupts
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x46);
    Wire.write(0xFF);
    Wire.endTransmission();
    // set target voltage and current of battery(axp173 datasheet PG.)
    // charge current (default)780mA -> 190mA
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x33);
    Wire.write(0xC1);
    Wire.endTransmission();
    // REG 10H: EXTEN & DC-DC2 control
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(AXP173_ADDR, 1, 1);
    int reg = Wire.read();
    Wire.beginTransmission(AXP173_ADDR);
    Wire.write(0x10);
    Wire.write(reg & 0xFC);
    Wire.endTransmission();
}

void setup()
{
    Serial.begin(115200);
    axp173_init();
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("init mobile net, load kmodel from SD card, it may takes a long time");
    if( mbnet.begin(kmodel_name) != 0)
    {
        Serial.println("mobile net init fail");
        while(1);
    }

}

void loop()
{
    if(mbnet.detect() != 0)
    {
        Serial.println("detect object fail");
        return;
    }
    mbnet.show();
}

