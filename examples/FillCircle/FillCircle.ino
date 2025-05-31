#include "Arduino_H7_Video.h"
#include "SDRAM.h"
#include "SpeeduinoGL.h"
#include "dsi.h"

uint16_t* FrameBuffer = (uint16_t*)SDRAM_START_ADDRESS;
const int ResV = 480;

Rectangle sq1 = {
    {0, 0},
    {0, 480},
    {800, 0},
    {800, 480}};

Point centre = {400, 240};

// The buffer used to rotate and resize the frame
Arduino_H7_Video Display(800, 480, GigaDisplayShield);

void setup() {
    SDRAM.begin();
    Display.begin();

    dsi_lcdClear(0);
    dsi_drawCurrentFrameBuffer();
    dsi_lcdClear(0);
    dsi_drawCurrentFrameBuffer();

    ConfigBuffer(SDRAM_START_ADDRESS, ResV);
    FillRectangle(sq1, 0x00FF);
}

void loop() {
    int32_t t1 = micros();
    // FillRectangle(sq1, 0x00FF);
    FillCircle(100, 0x0FFF, centre);
    Serial.println(micros() - t1);

    dsi_lcdDrawImage((void*)FrameBuffer, (void*)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
    dsi_drawCurrentFrameBuffer();
}
