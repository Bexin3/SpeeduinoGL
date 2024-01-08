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

  struct Rectangle {
    Point A;
    Point B;
    Point D;
    Point C;
  };

  void ConfigBuffer(uint32_t address = 0x60000000, uint32_t ResolutionV = 480, uint32_t ResolutionH = 800);
  void FillTriangle(Triangle triangle, uint16_t Colour);
  void FillRectangle(Rectangle rectangle, uint16_t Colour);

  void PolarizedTwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour, bool Polarity);
  void TwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour);
  void FillCircle(float Radius, uint16_t Colour, Point Centre);

#endif
