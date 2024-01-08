#include "SpeeduinoGL.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"

uint16_t *FrameBuffer = (uint16_t *)SDRAM_START_ADDRESS;
const int ResV = 480;
float rotation = 0;

  Square sq1 = {
  {0, 0},
  {0, 479},
  {799, 0},
  {799, 479}};

float angle = 0;
// The buffer used to rotate and resize the frame

// The buffer used to rotate and resize the frame
Arduino_H7_Video Display(800, 480, GigaDisplayShield);

void setup() {
  SDRAM.begin();
  Display.begin();
  // put your setup code here, to run once:
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();
  ConfigBuffer(SDRAM_START_ADDRESS, ResV);
}
void loop() {

  // put your main code here, to run repeatedly:
FillSquare(sq1, 0x0986);

  angle += 0.01;

  sq1 = {
  {400 + 150 * (- cos(angle) + sin(angle)), 240 + 150 * (cos(angle) + sin(angle))},
  {400 + 150 * (- cos(angle) - sin(angle)), 240 + 150 * (- cos(angle) + sin(angle))},
  {400 + 150 * (+ cos(angle) - sin(angle)), 240 + 150 * (- cos(angle) - sin(angle))},
  {400 + 150 * (+ cos(angle) + sin(angle)), 240 + 150 * (cos(angle) - sin(angle))}};

  long t1 = micros();
                                                             
FillSquare(sq1, 0x00FF);
  Serial.println(micros() - t1);
  dsi_lcdDrawImage((void *)FrameBuffer, (void *)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
  dsi_drawCurrentFrameBuffer();
}
