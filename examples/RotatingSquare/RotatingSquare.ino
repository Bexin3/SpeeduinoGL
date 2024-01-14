#include "SpeeduinoGL.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"

uint16_t* FrameBuffer = (uint16_t*)SDRAM_START_ADDRESS;
const int ResV = 480;

Rectangle sq1 = {
};

float angle = 0;

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
  
  FillScreen(0x0986);
}

void loop() {
  FillRectangle(sq1, 0x0986);

  angle += 0.01;

  sq1 = {
    {400 + 150 * (-cos(angle) + sin(angle)),  240 + 150 * (cos(angle)  + sin(angle)) },
    {400 + 150 * (-cos(angle) - sin(angle)),  240 + 150 * (-cos(angle) + sin(angle)) },
    {400 + 150 * (cos(angle)  - sin(angle)),  240 + 150 * (-cos(angle) - sin(angle)) },
    {400 + 150 * (cos(angle)  + sin(angle)),  240 + 150 * (cos(angle)  - sin(angle)) }
  };

  int32_t t1 = micros();
  FillRectangle(sq1, 0x00FF);
  Serial.println(micros() - t1);

  dsi_lcdDrawImage((void *)FrameBuffer, (void *)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
  dsi_drawCurrentFrameBuffer();
}
