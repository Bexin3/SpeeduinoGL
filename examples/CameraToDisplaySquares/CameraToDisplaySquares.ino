#include <algorithm>

#include "Arduino_H7_Video.h"
#include "SDRAM.h"
#include "SpeeduinoGL.h"
#include "arducam_dvp.h"
#include "dsi.h"

float ShiftH = -100;
float ShiftV = -400;
float zoom = 5;
float rotationRad = 0.3;

Rectangle sq1 = {
    {0, 0},
    {0, 480},
    {800, 0},
    {800, 480}};

uint16_t* DisplayFrameBuffer = (uint16_t*)SDRAM_START_ADDRESS;
FrameBuffer fb(1613300736);

#include "OV7670/ov767x.h"
// OV7670 ov767x;
OV7675 ov767x;
Camera cam(ov767x);
#define IMAGE_MODE CAMERA_RGB565

Arduino_H7_Video Display(800, 480, GigaDisplayShield);

void setup() {
    // put your setup code here, to run once:

    if (!cam.begin(CAMERA_R320x240, IMAGE_MODE, 30)) {
    }
    SDRAM.begin();
    Display.begin();

    dsi_lcdClear(0);
    dsi_drawCurrentFrameBuffer();
    dsi_lcdClear(0);
    dsi_drawCurrentFrameBuffer();

    FillRectangle(sq1, 0xFF00);
}

void loop() {
    // put your main code here, to run repeatedly:

    cam.grabFrame(fb, 3000);

    long t1 = micros();

    TransferSquares(ShiftH, ShiftV, zoom, rotationRad);

    Serial.println(micros() - t1);

    dsi_lcdDrawImage((void*)DisplayFrameBuffer, (void*)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
    dsi_drawCurrentFrameBuffer();
}
