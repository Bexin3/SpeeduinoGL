#include "arducam_dvp.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"
#include "lvgl.h"
#include <Arduino_GigaDisplay.h>
#include "Arduino_GigaDisplayTouch.h"
#include "SpeeduinoGL.h"

// This example only works with Greyscale cameras (due to the palette + resize&rotate algo)

int YValue = 480;
// The buffer used to capture the frame
//FrameBuffer fb(SDRAM_START_ADDRESS);
float zoom = 1;
float rotation = 0;
float shifti = 400; //Horizontal
float shiftj = 240; //Vertical

bool pause = 0;
float SafetyPixels = 0;

//Padding arround each frame to ensure fp innaccuracies dont leave areas uncleared

//Sensor resolution// 
const int resv = 320; 
const int resh = 240;

const int ResV = 320; 
const int ResH = 240;

GDTpoint_t PreviousPoints[2];
GDTpoint_t points[5];


RectangleRasterData testRast;
RectangleRasterData testRast2;

uint8_t contacts = 0;
uint8_t PreviousContacts = 0;

long testvar = 0;

//FrameBuffer outfb(SDRAM_START_ADDRESS+(320*240*2*8));
// The buffer used to rotate and resize the frame

FrameBuffer outfb(SDRAM_START_ADDRESS);
FrameBuffer fb(1613300736);


//Arduino_GigaDisplayTouch  TouchDetector;

// The buffer used to rotate and resize the frame
Arduino_H7_Video Display(800, 480, GigaDisplayShield);
GigaDisplayBacklight backlight;
Arduino_GigaDisplayTouch touchDetector;

uint32_t palette[256];

void setup() {

  //FrameReady = 00000000;
  SDRAM.begin();



    testRast=GetRasterData(0, 0, 1, 0, 800, 480);



  backlight.begin();

  Display.begin();

  // clear the display (gives a nice black background)
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();


  for (int i = 0; i < 800; i++) {
    for (int j = 0; j < 480; j++) {

      ((uint16_t*)outfb.getBuffer())[j + (i)*480] = 0x0986;
    };
  };



  touchDetector.begin();
  //lvll();
}



void loop() {

  int t1 = millis();

  UpdateTouch();

  float zoomcos = zoom * cos(rotation);
  float zoomsin = zoom * sin(rotation);

  float c1 = zoomcos;
  float c2 = zoomsin;
  float c3 = shiftj;
  float c4 = shifti;

    // Rotate the offset back to get the original camera origin position
    float cameraOriginX = (c1*c1*c4 - 120*c1 + c2*(c2*c4 + 160))/(c1*c1 + c2*c2);
    float cameraOriginY = (c1*c1*c3 - 160*c1 + c2*(c2*c3 - 120))/(c1*c1 + c2*c2);


testRast2 = GetRasterData(cameraOriginX+SafetyPixels*(cos(rotation)-sin(rotation))/zoom, cameraOriginY+SafetyPixels*(cos(rotation)+sin(rotation))/zoom, 1/zoom, rotation, ResH-2*SafetyPixels, ResV-2*SafetyPixels); //The factor added is due to FP inaccuracies
RectangleReplacement(testRast, testRast2, rand());
testRast = GetRasterData(cameraOriginX-SafetyPixels*(cos(rotation)-sin(rotation))/zoom, cameraOriginY-SafetyPixels*(cos(rotation)+sin(rotation))/zoom, 1/zoom, rotation, ResH+2*SafetyPixels, ResV+2*SafetyPixels); //The factor added is due to FP inaccuracies

//do the maths tommorow

  dsi_lcdDrawImage((void*)outfb.getBuffer(), (void*)dsi_getCurrentFrameBuffer(), 480, 800, DMA2D_INPUT_RGB565);


  dsi_drawCurrentFrameBuffer();




  Serial.println(millis() - t1);
}


void UpdateTouch() {


  PreviousPoints[0] = points[0];
  PreviousPoints[1] = points[1];
  PreviousContacts = contacts;
  contacts = touchDetector.getTouchPoints(points);

  if (contacts == 1 && PreviousContacts == 1) {
shifti += points[0].x - PreviousPoints[0].x;
shiftj += points[0].y - PreviousPoints[0].y;
  };

if (contacts == 2 && PreviousContacts == 2) {
    float currentMidX = (points[0].x + points[1].x) / 2.0;
    float currentMidY = (points[0].y + points[1].y) / 2.0;
    float previousMidX = (PreviousPoints[0].x + PreviousPoints[1].x) / 2.0;
    float previousMidY = (PreviousPoints[0].y + PreviousPoints[1].y) / 2.0;

    // Calculate the shift based on the movement of the midpoints
    shifti += (currentMidX - previousMidX);
    shiftj += (currentMidY - previousMidY);

    // Calculate the zoom factor
    double zoomfactor = sqrt(sq(points[0].x - points[1].x) + sq(points[0].y - points[1].y)) / 
                        sqrt(sq(PreviousPoints[0].x - PreviousPoints[1].x) + sq(PreviousPoints[0].y - PreviousPoints[1].y));

    // Adjust shift values based on the zoom factor and the midpoint
    shifti = currentMidX + (shifti - currentMidX) * zoomfactor;
    shiftj = currentMidY + (shiftj - currentMidY) * zoomfactor;

    // Update the zoom
    zoom /= zoomfactor;

    // Calculate the angle between the lines formed by the points
    float currentAngle = atan2(points[1].y - points[0].y, points[1].x - points[0].x);
    float previousAngle = atan2(PreviousPoints[1].y - PreviousPoints[0].y, PreviousPoints[1].x - PreviousPoints[0].x);

    // Calculate the rotation change
    float rotationM = currentAngle - previousAngle;

    // Normalize rotationM to be within -PI to PI
    if (rotationM > PI) {
        rotationM -= 2 * PI;
    } else if (rotationM < -PI) {
        rotationM += 2 * PI;
    }

    // Apply the rotation change
    rotation += rotationM;

    // Calculate the rotation point relative to the midpoint
    float rotationPointX = shifti - currentMidX;
    float rotationPointY = shiftj - currentMidY;

    // Rotate the rotation point
    float rotatedRotationPointX = rotationPointX * cos(rotationM) - rotationPointY * sin(rotationM);
    float rotatedRotationPointY = rotationPointX * sin(rotationM) + rotationPointY * cos(rotationM);

    // Adjust the shift values based on the rotated rotation point and the midpoint
    shifti = currentMidX + rotatedRotationPointX;
    shiftj = currentMidY + rotatedRotationPointY;
};
    
if (contacts == 3 && PreviousContacts == 3) {
zoom = 1;
rotation = 0;
shifti = 400; //Horizontal
shiftj = 240; //Vertical
testRast = GetRasterData(0, 0, 1, 0, 800, 480);
testRast2 = GetRasterData(0, 0, 1, 0, 0, 0);
RectangleReplacement(testRast, testRast2, rand());
testRast = testRast2;
};


//Serial.println(shiftj);
//Serial.println(zoom);

}
