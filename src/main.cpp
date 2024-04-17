
/*
  Based on Bodmer's library:
  https://github.com/Bodmer/TFT_eSPI

  Extension functions in the TFT_eFEX library are used to list SPIFFS files and render
  the jpeg to the TFT and to the Sprite:
  https://github.com/Bodmer/TFT_eFEX

  To render the Jpeg image the JPEGDecoder library is needed, this can be obtained
  with the IDE library manager, or downloaded from here:
  https://github.com/Bodmer/JPEGDecoder

  ==================================================================================*/

//====================================================================================
//                                  Libraries
//====================================================================================
#define DISABLE_ALL_LIBRARY_WARNINGS

#include <Arduino.h>

#include "dial_background.h"
#include "ball.h"
#include "plane_outline.h"
#include "LED_Images.h"

#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// Sprites used in display
TFT_eSprite mainSpr = TFT_eSprite(&tft);   // Main sprite. Full screen.

// The plane and the ball need to be sprites so they can be rotated and masked.
TFT_eSprite planeSpr = TFT_eSprite(&tft);   // Plane sprite
TFT_eSprite ballSpr = TFT_eSprite(&tft);    // Turn coordinator ball sprite

// The LEDs can just be overlayed.

// Create image buffers for double buffering
#define INSTRUMENT_WIDTH 320
#define INSTRUMENT_HEIGHT 300

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(1000); // wait for serial monitor to open
  Serial.println("\r\nTurn Coordinator PoC\n");

  tft.initDMA(); // Init DMA engine to speed up rendering

  tft.begin();
  tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
  // tft.setSwapBytes(true);
  //tft.pushImage(0, 0, dialWidth, dialHeight, dial);

  mainSpr.createSprite(INSTRUMENT_WIDTH, INSTRUMENT_HEIGHT);
  mainSpr.setSwapBytes(true);
  mainSpr.pushImage(0, 0, dialWidth, dialHeight, dial);

  // Create a sprite to hold the jpeg (or part of it)
  // CAC set impage depth.
  //planeSpr.setColorDepth(16);
  planeSpr.createSprite(planeOutlineWidth, planeOutlineHeight);
  planeSpr.setPivot(83, 22); // Determined in paint program. Plane rotates around the center of the fuselage.
  planeSpr.setSwapBytes(true);
  planeSpr.pushImage(0, 0, planeOutlineWidth, planeOutlineHeight, planeOutline);

  ballSpr.createSprite(ballWidth, ballHeight);
  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball_image);
  // The ball does not pivot, only moves x/y


  // readyLightSpr.createSprite(readyDotWidth, readyDotHeight);
  // readyLightSpr.pushImage(0, 0, readyDotWidth, readyDotHeight, ReadyDot);

  Serial.println("\r\nInitialisation done.\r\n");
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

  for (int16_t i = -60; i <= 60; i++)
  {
    int16_t angle = abs(i) - 30;

    mainSpr.pushImage(0, 0, dialWidth, dialHeight, dial); // Put a fresh background in the changing area to overwrite old plane.

    ballSpr.pushToSprite(&mainSpr, mainSpr.width() / 2 + angle - 10, mainSpr.height() - ballSpr.height() - round((abs(angle) / 5)) - 24, TFT_WHITE); 
    
    if (angle == 0) delay(2000);

    if (angle < 10)
    {
      mainSpr.pushImage(STDotX, STDotY, APDot_width, APDot_height, APDot);
      mainSpr.pushImage(HDDotX, HDDotY, APDot_width, APDot_height, APDot);
      mainSpr.pushImage(TrkLoDotX, TrkLoDotY, APDot_width, APDot_height, APDot);
      mainSpr.pushImage(TrkHiDotX, TrkHiDotY, APDot_width, APDot_height, APDot);

      mainSpr.pushImage(AltDot_x, AltDot_y, AltDot_width, AltDot_height, AltDot);

      mainSpr.pushImage(UpDot_x, UpDot_y, UpDot_width, UpDot_height, UpDot);
      mainSpr.pushImage(DownDot_x, DownDot_y, DownDot_width, DownDot_height, DownDot);

      mainSpr.pushImage(ReadyDot_x, ReadyDot_y, ReadyDot_width, ReadyDot_height, ReadyDot);

      mainSpr.pushImage(LowVoltFlag_x, LowVoltFlag_y, LowVoltFlag_width, LowVoltFlag_height, LowVoltFlag);

    }

    // Do the plane last. It's on top of all the others.
    mainSpr.setPivot(160, 150);                             // Set the pivot point for the rotation of the background
    planeSpr.pushRotated(&mainSpr, angle, TFT_WHITE);      // Push the plane sprite on the background

    mainSpr.pushSprite(0, 0); // Push the main sprite to the screen
    delay(10);
 
  }

}