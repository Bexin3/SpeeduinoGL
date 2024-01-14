#include "SpeeduinoGL.h"
#include <algorithm>

uint32_t ImageAddress = 0x60000000;
uint32_t ResV = 480;
uint32_t ResH = 800;

uint16_t InputSizeH = 240;
uint16_t InputSizeV = 320;
uint32_t CameraAddress = 1613300736;


  void ConfigBuffer(uint32_t Address, uint32_t ResolutionV, uint32_t ResolutionH) {
    ResV = ResolutionV;
    ImageAddress = Address;
    ResH = ResolutionH;
  }

void ConfigInput(uint32_t Address, uint32_t ResolutionV, uint32_t ResolutionH ) {
   InputSizeV = ResolutionV;
   CameraAddress = Address;
   InputSizeH = ResolutionH;
}

void FillScreen(uint16_t Colour) {
    TwoLineRasterizer(0, ResH, 0, ResV, 0, 0, Colour);
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

    DoubleFloat WouldWork =
    PolarizedTwoLineRasterizer(ceil(rectangle.A.w), ceil(rectangle.B.w),
                              rectangle.A.h + gradAD * (ceil(rectangle.A.w) - rectangle.A.w),
                              rectangle.A.h + gradAB * (ceil(rectangle.A.w) - rectangle.A.w),
                              gradAB, gradAD, Colour, Polarized);
      

      
    if (Polarized) { std::swap(WouldWork.Float1, WouldWork.Float2); };

    if (switched) {
       WouldWork =
      PolarizedTwoLineRasterizer(ceil(rectangle.B.w), ceil(rectangle.C.w),
                                WouldWork.Float2,
                                rectangle.B.h + gradBC * (ceil(rectangle.B.w) - rectangle.B.w),
                                gradBC, gradAD, Colour, Polarized);
        
      if (Polarized) { std::swap(WouldWork.Float1, WouldWork.Float2); };

      PolarizedTwoLineRasterizer(ceil(rectangle.C.w), ceil(rectangle.D.w),
                                WouldWork.Float2,
                                rectangle.C.h + gradDC * (ceil(rectangle.C.w) - rectangle.C.w),
                                gradDC, gradAD, Colour, Polarized);
    } else {
       WouldWork =
      PolarizedTwoLineRasterizer(ceil(rectangle.B.w), ceil(rectangle.D.w),
                                WouldWork.Float2,
                                rectangle.B.h + gradBC * (ceil(rectangle.B.w) - rectangle.B.w),
                                gradBC, gradAD, Colour, Polarized);
        
        if (Polarized) { std::swap(WouldWork.Float1, WouldWork.Float2); };

      PolarizedTwoLineRasterizer(ceil(rectangle.D.w), ceil(rectangle.C.w),
                                rectangle.D.h + gradDC * (ceil(rectangle.D.w) - rectangle.D.w),
                                WouldWork.Float1,
                                gradBC, gradDC, Colour, Polarized);
    }
  }

DoubleFloat PolarizedTwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour, bool Polarity) {
    if (Polarity) {
      return(TwoLineRasterizer(CellStartX, CellEndX, PointerCoordinateH, PointerEndH, Gradient1, Gradient2, Colour));
    } else {
      return(TwoLineRasterizer(CellStartX, CellEndX, PointerEndH, PointerCoordinateH, Gradient2, Gradient1, Colour));
    }
  }


DoubleFloat TwoLineRasterizer(int32_t CellStartX, int32_t CellEndX, float PointerCoordinateH, float PointerEndH, float Gradient1, float Gradient2, uint16_t Colour) {

    
    uint16_t* ImageBuffer = (uint16_t*)ImageAddress;


    if (CellStartX < 0) {
        if (CellEndX < 0) {
            PointerCoordinateH -= Gradient2 * (CellStartX-CellEndX);
            PointerEndH -= Gradient1 * (CellStartX-CellEndX);
            goto returnpoint;
        } else {
            PointerCoordinateH -= Gradient2 * CellStartX;
            PointerEndH -= Gradient1 * CellStartX;
            CellStartX = 0;
        };
    };
    


    
    
      

    if (CellEndX > ResH) {
        CellEndX = ResH;
    };

        
    for (uint32_t CurrentW = CellStartX; CellEndX > CurrentW; CurrentW++) {
      int32_t PointerCoorInt = ceil(PointerCoordinateH);
      int32_t PointerEndInt = ceil(PointerEndH);
        
        
        
        if (PointerEndInt < 0) {
          PointerEndInt = 0;
        };
        
        if (PointerCoorInt < 0) {
            PointerCoorInt = 0;
        };


        
      if (PointerEndInt > ResV) {
        PointerEndInt = ResV;
      };
        
        if (PointerCoorInt > ResV) {
            PointerCoorInt = ResV;
        };
        

      for (uint32_t CurrentH = PointerCoorInt; PointerEndInt > CurrentH; CurrentH++) {
        ImageBuffer[ResV * (CurrentW) + (CurrentH)] = Colour;
      };

      PointerCoordinateH += Gradient2;
      PointerEndH += Gradient1;
    };
    
    
    
    returnpoint:
    
    DoubleFloat Retruner = {PointerCoordinateH, PointerEndH};
    
    return(Retruner);
    
    
    
    
    
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
     

void TransferSquares(float ShiftH, float ShiftV, float zoom, float rotationRad) {
    
      uint16_t* InputBuffer = (uint16_t*)CameraAddress;

      float gradient1 = cos(rotationRad);
      float gradient2 = sin(rotationRad);


      float HShift = zoom * gradient2;
      float VShift = zoom * gradient1;

      gradient1 = abs(gradient1);
      gradient2 = abs(gradient2);

      float grad1;
      float grad2;



      bool Polarized = 0;

      if (sin(2 * rotationRad) <= 0) {
        std::swap(gradient1, gradient2);
      };

      if (sin(4 * rotationRad) <= 0) {
          Polarized = 1;
          Serial.println("test");
        grad1 = gradient2 / gradient1;
        grad2 = -gradient1 / gradient2;
      } else {
        grad1 = -gradient1 / gradient2;
        grad2 = gradient2 / gradient1;
      };



      float CellPointerH = ShiftH;
      float CellPointerV = ShiftV;

      const float BoundStartH = 0 - zoom - abs(VShift);
      const float BoundStartV = 0 - zoom - abs(HShift);
      const float BoundEndH = ResH + zoom + abs(VShift);
      const float BoundEndV = ResV + zoom + abs(HShift);


      for (int16_t PosH = 0; PosH < InputSizeH; PosH++) {


        float CellPointerHU = CellPointerH + VShift;
        float CellPointerVU = CellPointerV + HShift;

        float EOCPTRH = CellPointerHU;
        float EOCPTRV = CellPointerVU;

        float CellPointerH2 = CellPointerH - HShift;
        float CellPointerV2 = CellPointerV + VShift;
        float CellPointerHU2 = CellPointerHU - HShift;
        float CellPointerVU2 = CellPointerVU + VShift;



        for (int16_t PosV = 0; PosV < InputSizeV; PosV++) {

          if (CellPointerH <= BoundEndH && CellPointerV <= BoundEndV && CellPointerH >= BoundStartH && CellPointerV >= BoundStartV) {



            Rectangle square = {
              { CellPointerH, CellPointerV },
              { CellPointerHU, CellPointerVU },
              { CellPointerH2, CellPointerV2 },
              { CellPointerHU2, CellPointerVU2 }
            };




            uint16_t Colour = InputBuffer[InputSizeV * PosH + PosV];
            Colour = (((Colour >> 8) & 0x00FF) | ((Colour << 8) & 0xFF00));



            std::sort(&square.A, &square.C + 1,
                      [](const Point &a, const Point &b) {
                        return a.w < b.w;
                      });

            
            if (square.A.w == square.B.w) {
                if (Polarized == (square.A.h > square.B.h)) {
                    std::swap(square.A.h, square.B.h);
                }
                
                
                if (square.A.h != square.D.h) {
                  std::swap(square.D.h, square.C.h);
                };
                  
            };
              
            


            DoubleFloat WouldWork =
              PolarizedTwoLineRasterizer(ceil(square.A.w), ceil(square.B.w),
                                         square.A.h + grad2 * (ceil(square.A.w) - square.A.w),
                                         square.A.h + grad1 * (ceil(square.A.w) - square.A.w),
                                         grad1, grad2, Colour, Polarized);

            if (Polarized) { std::swap(WouldWork.Float1, WouldWork.Float2); };
            WouldWork =
              PolarizedTwoLineRasterizer(ceil(square.B.w), ceil(square.D.w),
                                         WouldWork.Float2,
                                         square.B.h + grad2 * (ceil(square.B.w) - square.B.w),
                                         grad2, grad2, Colour, Polarized);

            if (Polarized) { std::swap(WouldWork.Float1, WouldWork.Float2); };
              PolarizedTwoLineRasterizer(ceil(square.D.w), ceil(square.C.w),
                                         square.D.h + grad1 * (ceil(square.D.w) - square.D.w),
                                         WouldWork.Float1,
                                         grad2, grad1, Colour, Polarized);
          };

          CellPointerH = CellPointerH2;
          CellPointerV = CellPointerV2;
            
          CellPointerHU = CellPointerHU2;
          CellPointerVU = CellPointerVU2;

          CellPointerH2 -= HShift;
          CellPointerV2 += VShift;

          CellPointerHU2 -= HShift;
          CellPointerVU2 += VShift;
        };

        CellPointerH = EOCPTRH;
        CellPointerV = EOCPTRV;
      };
    }
