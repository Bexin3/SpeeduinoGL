
#include "arducam_dvp.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"
#include "OV7670/ov767x.h"
#include <algorithm>

OV7675 ov767x;
Camera cam(ov767x);
#define IMAGE_MODE CAMERA_RGB565


FrameBuffer outfb(SDRAM_START_ADDRESS);
uint16_t *FrameBuffer = (uint16_t *)SDRAM_START_ADDRESS; //Manual pointer
const int ResH = 480;
float rotation = 0;

struct Point {
  float w;
  float h;
};

struct Triangle {
  Point A;
  Point B;
  Point C;
};

struct Square {
  Point A;
  Point B;
  Point D;
  Point C;
};

  Triangle myT = { { 200, 50 }, { 150, 150 }, { 200, 150 } };
  Square sq1 = {
  {200, 400},
  {250, 150},
  {600, 300},
  {600, 400}};
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
  // put your main code here, to run repeatedly:
//myT = { { byte(rand()), byte(rand()) }, { byte(rand()), byte(rand()) }, { byte(rand()), byte(rand()) } };
  byte ranloc = millis();
  byte ranloc2 = micros();
  angle += 0.01;

  sq1 = {
  {400 + 50 * (- cos(angle) + sin(angle)), 240 + 50 * (cos(angle) + sin(angle))},
  {400 + 50 * (- cos(angle) - sin(angle)), 240 + 50 * (- cos(angle) + sin(angle))},
  {400 + 50 * (+ cos(angle) - sin(angle)), 240 + 50 * (- cos(angle) - sin(angle))},
  {400 + 50 * (+ cos(angle) + sin(angle)), 240 + 50 * (cos(angle) - sin(angle))}};

  for (int i = 0; i < 800; i++) {
    for (int j = 0; j < 480; j++) {

      ((uint16_t *)outfb.getBuffer())[j + (i)*480] = 0x0986;
    };
  };

  long t1 = micros();
                                                             
 //FillTriangle(ranloc, 200, ranloc2, 50, 400, 100, 0xFF0F);
 // FillTriangle(20, 20, 35, 35, 40, 10, 0xF00F);

  // FillTriangle(20, 20, 35, 35, 40, 10, 0xF00F);
// FillTriangle(myT, 0xFF0F);
  //dsi_lcdFillArea((void*)dsi_getCurrentFrameBuffer(), 80, 80, DMA2D_INPUT_RGB565);
//FillSquare(sq1, 0xF00F);
FillSquare(sq1, 0x00FF);

  Serial.println(micros() - t1);
  dsi_lcdDrawImage((void *)outfb.getBuffer(), (void *)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);
  dsi_drawCurrentFrameBuffer();

}




//Left to right
//Top to bottom
void FillTriangle(Triangle triangle, uint16_t Colour) {




  //Point square.C = { { Aw, Ah }, { Bw, Bh }, { Cw, Ch } };

  // Sort points based on x-coordinates
  std::sort(&triangle.A, &triangle.C + 1, [](const Point &a, const Point &b) {
    return a.w < b.w;
  });


  int dAwBw = floor(triangle.B.w) - ceil(triangle.A.w);
  int dBwCw = floor(triangle.C.w) - ceil(triangle.B.w);

  float gradAC = (triangle.A.h - triangle.C.h) / (triangle.A.w - triangle.C.w);
  float gradAB = (triangle.A.h - triangle.B.h) / (triangle.A.w - triangle.B.w);
  float gradBC = (triangle.B.h - triangle.C.h) / (triangle.B.w - triangle.C.w);


  if (triangle.B.h > triangle.C.h || triangle.A.w == triangle.B.w) {
    TwoLineRasterizer(dAwBw, triangle.A.h + gradAC * (ceil(triangle.A.w) - triangle.A.w), triangle.A.h + gradAB * (ceil(triangle.A.w) - triangle.A.w), gradAB, gradAC, triangle.A.w, 0, Colour);
    TwoLineRasterizer(dBwCw, triangle.A.h + gradAC * (floor(triangle.B.w) - triangle.A.w), triangle.B.h + gradBC * (ceil(triangle.B.w) - triangle.B.w), gradBC, gradAC, triangle.B.w, 0, Colour);
  } else {
    TwoLineRasterizer(dAwBw, triangle.A.h + gradAB * (ceil(triangle.A.w) - triangle.A.w), triangle.A.h + gradAC * (ceil(triangle.A.w) - triangle.A.w), gradAC, gradAB, triangle.A.w, 0, Colour);
    TwoLineRasterizer(dBwCw, triangle.B.h + gradBC * (ceil(triangle.B.w) - triangle.B.w), triangle.A.h + gradAC * (floor(triangle.B.w) - triangle.A.w), gradAC, gradBC, triangle.B.w, 0, Colour);
  };

  //TwoLineRasterizer(50, 166, 300, -1, -0.3333, triangle.B.w, 0, Colour);
}




void FillSquare(Square square, uint16_t Colour) {


  // Sort points based on x-coordinates
  std::sort(&square.A, &square.C + 1, [](const Point &a, const Point &b) {
    return a.w < b.w;
  });


  if (square.A.w == square.B.w) {
    if ((square.A.h > square.B.h && square.C.h < square.D.h) || (square.A.h < square.B.h && square.C.h > square.D.h)) {
      std::swap(square.A.h, square.B.h);

 
  };
  };
 


  int dAwBw = floor(square.B.w) - ceil(square.A.w);
  int dBwDw = floor(square.D.w) - ceil(square.B.w);
  int dDwCw = floor(square.C.w) - ceil(square.D.w);


  float gradAD = (square.A.h - square.D.h) / (square.A.w - square.D.w);
  float gradAB = (square.A.h - square.B.h) / (square.A.w - square.B.w);
  float gradBC = (square.B.h - square.C.h) / (square.B.w - square.C.w);
  float gradDC = (square.D.h - square.C.h) / (square.D.w - square.C.w);


//Replace grad AD reuse code if possible
//Try to go wide if theres a split low gradient triangle middle point

  if (square.B.h > square.D.h) {
    TwoLineRasterizer(dAwBw, square.A.h + gradAD * (ceil(square.A.w) - square.A.w), square.A.h + gradAB * (ceil(square.A.w) - square.A.w), gradAB, gradAD, square.A.w, 0, 0xF00F);
    
    TwoLineRasterizer(dBwDw, square.A.h + gradAD * (floor(square.B.w) - square.A.w), square.B.h + gradBC * (ceil(square.B.w) - square.B.w), gradBC, gradAD, square.B.w, 0, 0xFF0F);

    TwoLineRasterizer(dDwCw, square.A.h + gradAD * (floor(square.D.w) - square.A.w), square.B.h + gradBC * (ceil(square.D.w) - square.B.w), gradBC, gradDC, square.D.w, 0, 0x0FFF);

  } else {

    TwoLineRasterizer(dAwBw, square.A.h + gradAB * (ceil(square.A.w) - square.A.w), square.A.h + gradAD * (ceil(square.A.w) - square.A.w), gradAD, gradAB, square.A.w, 0, 0xF00F);
    
    TwoLineRasterizer(dBwDw, square.B.h + gradBC * (ceil(square.B.w) - square.B.w), square.A.h + gradAD * (floor(square.B.w) - square.A.w), gradAD, gradBC, square.B.w, 0, 0xFF0F);

    TwoLineRasterizer(dDwCw, square.B.h + gradBC * (ceil(square.D.w) - square.B.w), square.A.h + gradAD * (floor(square.D.w) - square.A.w), gradDC, gradBC, square.D.w, 0, 0x0FFF);

  };

  //TwoLineRasterizer(50, 166, 300, -1, -0.3333, square.B.w, 0, Colour);
}


void TwoLineRasterizer(int CellSizeX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, int pointerToCellW, int pointerToCellH, uint16_t Colour) {


  for (uint CurrentW = 0; CellSizeX > CurrentW; CurrentW++) {

    int PointerCoorInt = PointerCoordinateH;
    int PointerEndInt = PointerEndH;

    for (int CurrentH = PointerCoorInt; PointerEndInt > CurrentH; CurrentH++) {
      FrameBuffer[ResH * (CurrentW + pointerToCellW) + (CurrentH + pointerToCellH)] = Colour;
    };


    PointerCoordinateH += Gradient2;
    PointerEndH += Gradient1;
  };
}
