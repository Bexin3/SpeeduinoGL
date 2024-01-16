#ifndef SpeeduinoGL_h
#define SpeeduinoGL_h

#include <Arduino.h>

  struct Point {
    float w;
    float h;
  };


struct DoubleFloat {
  float Float1;
  float Float2;
};

  struct Triangle {
    Point A;
    Point B;
    Point C;
  };

  struct Rectangle {
    Point A;
    Point B;
    Point D;
    Point C;
  };




  void ConfigBuffer(uint32_t address = 0x60000000, uint32_t ResolutionV = 480, uint32_t ResolutionH = 800);
  void ConfigInput(uint32_t address = 1613300736, uint32_t ResolutionV = 320, uint32_t ResolutionH = 240);
  void FillTriangle(Triangle triangle, uint16_t Colour);
  void FillRectangle(Rectangle rectangle, uint16_t Colour);

  void FillScreen(uint16_t Colour);

__attribute__((always_inline))
DoubleFloat PolarizedTwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour, bool Polarity);
__attribute__((always_inline))
DoubleFloat TwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour);


void FillCircle(float Radius, uint16_t Colour, Point Centre);

void TransferSquares(float ShiftH, float ShiftV, float zoom, float rotationRad);

#endif
