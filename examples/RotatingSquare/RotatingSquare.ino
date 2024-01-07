#include "SpeeduinoGL.h"
#include "arducam_dvp.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"
#include "OV7670/ov767x.h"


OV7675 ov767x;
Camera cam(ov767x);
#define IMAGE_MODE CAMERA_RGB565


FrameBuffer outfb(SDRAM_START_ADDRESS);
uint16_t *FrameBuffer = (uint16_t *)SDRAM_START_ADDRESS;
const int ResV = 480;
float rotation = 0;



  Triangle myT = { { 200, 50 }, { 150, 150 }, { 300, 150 } };
  Square sq1 = {
  {1, 1},
  {1, 479},
  {799, 1},
  {799, 479}};
  Square sq2 = {{700, 200}, {700, 250}, {100, 100}, {200, 150}};

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


  for (int i = 0; i < 800; i++) {
    for (int j = 0; j < 480; j++) {

      ((uint16_t *)outfb.getBuffer())[j + (i)*480] = 0x0986;
    };
  };
}

void loop() {


  ConfigBuffer(SDRAM_START_ADDRESS, ResV);
  // put your main code here, to run repeatedly:
//myT = { { byte(rand()), byte(rand()) }, { byte(rand()), byte(rand()) }, { byte(rand()), byte(rand()) } };
  byte ranloc = millis();
  byte ranloc2 = micros();
  angle += 0.01;

  sq1 = {
  {400 + 150 * (- cos(angle) + sin(angle)), 240 + 150 * (cos(angle) + sin(angle))},
  {400 + 150 * (- cos(angle) - sin(angle)), 240 + 150 * (- cos(angle) + sin(angle))},
  {400 + 150 * (+ cos(angle) - sin(angle)), 240 + 150 * (- cos(angle) - sin(angle))},
  {400 + 150 * (+ cos(angle) + sin(angle)), 240 + 150 * (cos(angle) - sin(angle))}};


  for (int i = 0; i < 800; i++) {
    for (int j = 0; j < 480; j++) {

      ((uint16_t *)outfb.getBuffer())[j + (i)*480] = 0x0986;
    };
  };

  long t1 = micros();
                                                             

FillSquare(sq1, 0x00FF);
 FillTriangle(myT, 0xFF0F);

  Serial.println(micros() - t1);
  dsi_lcdDrawImage((void *)outfb.getBuffer(), (void *)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
  dsi_drawCurrentFrameBuffer();


}

