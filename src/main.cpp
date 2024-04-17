
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
// Call up the SPIFFS FLASH filing system, this is part of the ESP Core
#define FS_NO_GLOBALS
#include "LittleFS.h"
#include "dial_background.h"
#include "turn_background.h"
#include "ball_background.h"
#include "lights_bg.h"
#include "ready_light.h"
#include "ball.h"
#include "plane_outline.h"
#include "over_write.h"

#include <TFT_eSPI.h>                  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();             // Invoke custom library

// Sprites used in display
TFT_eSprite planeSpr = TFT_eSprite(&tft);   // Plane sprite
TFT_eSprite planeBgSpr = TFT_eSprite(&tft);      // Plane Background sprite
TFT_eSprite ballSpr = TFT_eSprite(&tft);    // Turn coordinator ball sprite
TFT_eSprite ballBgSpr = TFT_eSprite(&tft);  // Turn coordinator ball background sprite
TFT_eSprite lightsBgSpr = TFT_eSprite(&tft); 
TFT_eSprite readyLightSpr = TFT_eSprite(&tft); 


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
  
  tft.begin();
  tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, dialWidth, dialHeight, dial);

  // Create a sprite to hold the jpeg (or part of it)
  // CAC set impage depth.
  planeSpr.setColorDepth(16);
  planeSpr.createSprite(planeOutlineWidth, planeOutlineHeight);
  planeSpr.setPivot(83, 22);  // Determined in paint program. Plane rotates around the center of the fuselage.
  planeSpr.setSwapBytes(true);
  planeSpr.pushImage(0, 0, planeOutlineWidth, planeOutlineHeight, planeOutline);

  planeBgSpr.createSprite(turnOverWriteWidth, turnOverWriteHeight);
  planeBgSpr.setSwapBytes(true);
  planeBgSpr.setPivot(91, 50); // Where the plane's pivot point is on the background sprite.

  ballSpr.createSprite(ballWidth, ballHeight);    
  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball_image);
  // The ball does not pivot, only moves x/y

  ballBgSpr.createSprite(ballBackgroundwidth, ballBackgroundHeight);
  ballBgSpr.setSwapBytes(true);

  lightsBgSpr.createSprite(LightsBgWidth, LightsBgHeight);
  lightsBgSpr.setSwapBytes(true);
  
  readyLightSpr.createSprite(readyDotWidth, readyDotHeight);
  readyLightSpr.pushImage(0, 0, readyDotWidth, readyDotHeight, ReadyDot);

  Serial.println("\r\nInitialisation done.\r\n");
  
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

 
  for (int16_t angle = -30; angle <= 30; angle += 1)
  {
    planeBgSpr.pushImage(0, 0, turnOverWriteWidth, turnOverWriteHeight, turnOverwrite);   // Put a fresh background in the changing area to overwrite old plane.
    planeSpr.pushRotated(&planeBgSpr, angle, TFT_WHITE);  // Push the plane sprite on the background
    planeBgSpr.pushSprite(69, 96);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    ballBgSpr.pushImage(0, 0, ballBackgroundwidth, ballBackgroundHeight, ball_background);   // Put a fresh background in the changing area to overwrite old plane.
    ballSpr.pushToSprite(&ballBgSpr, ballBgSpr.width()/2 + angle - 10, ballBgSpr.height() - ballSpr.height() - round((abs(angle)/5)) + 0, TFT_WHITE);  // Push the plane sprite on the background
    ballBgSpr.pushSprite(97,236);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    lightsBgSpr.pushImage(0, 0, LightsBgWidth, LightsBgHeight, LightsBg);   // Put a fresh background in the changing area to overwrite old lights
    if(angle < 10)
    {
    readyLightSpr.pushToSprite(&lightsBgSpr, 1, 49, TFT_WHITE);  // Push the LED sprite on the Lights background
    }
    lightsBgSpr.pushSprite(lightsBgX,lightsBgY);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    delay(50);
  }
  for (int16_t angle = 30; angle >= -30; angle -= 1)
  {
  planeBgSpr.pushImage(0, 0, turnOverWriteWidth, turnOverWriteHeight, turnOverwrite);
    planeSpr.pushRotated(&planeBgSpr, angle, TFT_WHITE);
    planeBgSpr.pushSprite(69,96);

    ballBgSpr.pushImage(0, 0, ballBackgroundwidth, ballBackgroundHeight, ball_background);   // Put a fresh background in the changing area to overwrite old plane.
    ballSpr.pushToSprite(&ballBgSpr, ballBgSpr.width()/2 + angle - 10, ballBgSpr.height() - ballSpr.height() - round((abs(angle)/5)) + 0, TFT_WHITE);  // Push the plane sprite on the background
    ballBgSpr.pushSprite(97,236);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    delay(50);
  }

  delay(2000);

}