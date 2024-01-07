
#ifndef SpeeduinoGL_h
#define SpeeduinoGL_h

#include <Arduino.h>


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

void ConfigBuffer(long address, uint ResolutionV);
void FillTriangle(Triangle triangle, uint16_t Colour);
void FillSquare(Square square, uint16_t Colour);
void PolarizedTwoLineRasterizer(int CellStartX, int CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour, bool Polarity);
void TwoLineRasterizer(int CellStartX, int CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour);

#endif