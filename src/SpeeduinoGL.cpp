#include "SpeeduinoGL.h"
#include <algorithm>

uint32_t ImageAddress = 0x60000000;
uint32_t ResV = 480;
uint32_t ResH = 800;


  void ConfigBuffer(uint32_t Address, uint32_t ResolutionV, uint32_t ResolutionH) {
    ResV = ResolutionV;
    ImageAddress = Address;
    ResH = ResolutionH;
  }

  void FillTriangle(Triangle triangle, uint16_t Colour) {
    // Sort points based on x-coordinates
    std::sort(&triangle.A, &triangle.C + 1,
      [](const Point &a, const Point &b) {
        return a.w < b.w;
      }
    );

    float gradAC = (triangle.A.h - triangle.C.h) / (triangle.A.w - triangle.C.w);
    float gradAB = (triangle.A.h - triangle.B.h) / (triangle.A.w - triangle.B.w);
    float gradBC = (triangle.B.h - triangle.C.h) / (triangle.B.w - triangle.C.w);
    
    bool Polarized = gradAB > gradAC;

    PolarizedTwoLineRasterizer(ceil(triangle.A.w), ceil(triangle.B.w),
                              triangle.A.h + gradAC * (ceil(triangle.A.w) - triangle.A.w),
                              triangle.A.h + gradAB * (ceil(triangle.A.w) - triangle.A.w),
                              gradAB, gradAC, Colour, Polarized);

    PolarizedTwoLineRasterizer(ceil(triangle.B.w), ceil(triangle.C.w),
                              triangle.A.h + gradAC * (ceil(triangle.B.w) - triangle.A.w),
                              triangle.B.h + gradBC * (ceil(triangle.B.w) - triangle.B.w),
                              gradBC, gradAC, Colour, Polarized);
  }

  void FillRectangle(Rectangle rectangle, uint16_t Colour) {
    // Sort points based on x-coordinates
    std::sort(&rectangle.A, &rectangle.C + 1,
      [](const Point &a, const Point &b) {
        return a.w < b.w;
      }
    );

    bool switched = false;
    bool Polarized = false;

    float gradAC = (rectangle.C.h - rectangle.A.h) / (rectangle.C.w - rectangle.A.w);
    float gradAD = (rectangle.D.h - rectangle.A.h) / (rectangle.D.w - rectangle.A.w);
    float gradAB = (rectangle.B.h - rectangle.A.h) / (rectangle.B.w - rectangle.A.w);

    if (gradAD > gradAB) {
      if (gradAC > gradAD) {
        std::swap(rectangle.C, rectangle.D);
        std::swap(gradAC, gradAD);
        switched = true;
      }
    } else {
      if (gradAC < gradAD) {
        std::swap(rectangle.C, rectangle.D);
        std::swap(gradAC, gradAD);
        switched = true;
      }
    }

    float gradBC = (rectangle.C.h - rectangle.B.h) / (rectangle.C.w - rectangle.B.w);
    float gradDC = (rectangle.C.h - rectangle.D.h) / (rectangle.C.w - rectangle.D.w);

    Polarized = gradAB > gradAD;

    PolarizedTwoLineRasterizer(ceil(rectangle.A.w), ceil(rectangle.B.w),
                              rectangle.A.h + gradAD * (ceil(rectangle.A.w) - rectangle.A.w),
                              rectangle.A.h + gradAB * (ceil(rectangle.A.w) - rectangle.A.w),
                              gradAB, gradAD, Colour, Polarized);

    if (switched) {
      PolarizedTwoLineRasterizer(ceil(rectangle.B.w), ceil(rectangle.C.w), 
                                rectangle.A.h + gradAD * (ceil(rectangle.B.w) - rectangle.A.w),
                                rectangle.B.h + gradBC * (ceil(rectangle.B.w) - rectangle.B.w),
                                gradBC, gradAD, Colour, Polarized);

      PolarizedTwoLineRasterizer(ceil(rectangle.C.w), ceil(rectangle.D.w),
                                rectangle.A.h + gradAD * (ceil(rectangle.C.w) - rectangle.A.w),
                                rectangle.C.h + gradDC * (ceil(rectangle.C.w) - rectangle.C.w),
                                gradDC, gradAD, Colour, Polarized);
    } else {
      PolarizedTwoLineRasterizer(ceil(rectangle.B.w), ceil(rectangle.D.w),
                                rectangle.A.h + gradAD * (ceil(rectangle.B.w) - rectangle.A.w),
                                rectangle.B.h + gradBC * (ceil(rectangle.B.w) - rectangle.B.w),
                                gradBC, gradAD, Colour, Polarized);

      PolarizedTwoLineRasterizer(ceil(rectangle.D.w), ceil(rectangle.C.w),
                                rectangle.D.h + gradDC * (ceil(rectangle.D.w) - rectangle.D.w),
                                rectangle.B.h + gradBC * (ceil(rectangle.D.w) - rectangle.B.w),
                                gradBC, gradDC, Colour, Polarized);
    }
  }

  void PolarizedTwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour, bool Polarity) {
    if (Polarity) {
      TwoLineRasterizer(CellStartX, CellEndX, PointerCoordinateH, PointerEndH, Gradient1, Gradient2, Colour);
    } else {
      TwoLineRasterizer(CellStartX, CellEndX, PointerEndH, PointerCoordinateH, Gradient2, Gradient1, Colour);
    }
  }


  void TwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour) {
    uint16_t* ImageBuffer = (uint16_t*)ImageAddress;
      
    if (CellStartX < 0) {
        PointerCoordinateH -= Gradient2 * CellStartX;
        PointerEndH -= Gradient1 * CellStartX;
    }
       
    if (CellEndX > ResH) {
        CellEndX = ResH;
    }

    for (uint32_t CurrentW = CellStartX; CellEndX > CurrentW; CurrentW++) {
      uint32_t PointerCoorInt = ceil(PointerCoordinateH);
      uint32_t PointerEndInt = ceil(PointerEndH);
        
      if (PointerEndInt > ResV) {
        PointerEndInt = ResV;
      }

      for (int32_t CurrentH = PointerCoorInt; PointerEndInt > CurrentH; CurrentH++) {
        ImageBuffer[ResV * (CurrentW) + (CurrentH)] = Colour;
      }

      PointerCoordinateH += Gradient2;
      PointerEndH += Gradient1;
    }
  }

    void FillCircle(float Radius, uint16_t Colour, Point Centre) {
      uint16_t* ImageBuffer = (uint16_t*)ImageAddress;
     
      float RadiusTo2 = pow(Radius, 2);
     
      uint32_t CellEndX = ceil(Centre.w+Radius);
     
      if (CellEndX > ResH) {
         CellEndX = ResH;
      };
     
      uint32_t CellStartX = ceil(Centre.w-Radius);
      float RadiusPos = CellStartX-(Centre.w);
     
     
      for (uint32_t CurrentW = CellStartX; CellEndX > CurrentW; CurrentW++) {
         
         
         float height = sqrt(RadiusTo2 - pow(RadiusPos, 2));
         uint16_t PointerCoorInt = ceil(Centre.h - height);
         uint16_t PointerEndInt = ceil(Centre.h + height);
         
         
         
        if (PointerEndInt > ResV) {
          PointerEndInt = ResV;
        }

        for (int32_t CurrentH = PointerCoorInt; (PointerEndInt) > CurrentH; CurrentH++) {
         ImageBuffer[ResV * (CurrentW) + (CurrentH)] = Colour;
        };
         
        RadiusPos++;

     }
    }
     
