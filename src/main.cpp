
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

#include "dial_image.h"
#include "ball_image.h"
#include "plane_image.h"
#include "LED_Images.h"

#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// Sprites used in display
TFT_eSprite mainSpr = TFT_eSprite(&tft); // Main sprite. Full screen.

// The plane and the ball need to be sprites so they can be rotated and masked.
TFT_eSprite planeSpr = TFT_eSprite(&tft); // Plane sprite
TFT_eSprite ballSpr = TFT_eSprite(&tft);  // Turn coordinator ball sprite

// The LEDs can just be overlayed.

// Create image buffers for double buffering
#define INSTRUMENT_WIDTH 320
#define INSTRUMENT_HEIGHT 300

// State Variables
// LEDs
bool altLedState = false,
     rdyLedState = false,
     lowVoltLedState = false,
     upLedState = false,
     downLedState = false,
     stLedState = false,
     hdLedState = false,
     trkLoLedState = false,
     trkHiLedState = false;
// Turn Coordinator Needle
double turnCoorNeedle = 50; // 0 to 100, 50 is centered.
// Ball
double inclinometerBall = 0;  // -100 to 100 with 0 being centered.

void setTurnCoordNeedle(double percent);
void setInclinometerBall(double percent);
void setApTrimUpLight(bool state);
void setApTrimDownLight(bool state);
void setApTrkLoLight(bool state);
void setApTrkHiLight(bool state);
void setApStLight(bool state);
void setApRdyLight(bool state);
void setApHdLight(bool state);
void setApAltLight(bool state);
void setLowVoltLight(bool state);


void displayLeds();
void displayTurnCoordNeedle();
void displayBall();

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

  // Turn on the rp2040 LED.
  // This will eventually get moved into Mobiflight, and it uses the Serial port,
  // so the build-in LED can be used for very crude debugging. 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  // tft.initDMA(); // Init DMA engine to speed up rendering

  tft.begin();
  tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK); // Clear screen. We are only going to use the top part. If you don't clear, the bottom half will be noise.

  mainSpr.createSprite(INSTRUMENT_WIDTH, INSTRUMENT_HEIGHT);
  mainSpr.setSwapBytes(true);
  mainSpr.setPivot(160, 150);                       // Set the pivot point for the rotation of the background

  // Create a sprite to hold the jpeg (or part of it)
  planeSpr.createSprite(planeOutlineWidth, planeOutlineHeight);
  planeSpr.setPivot(planeCenterX, planeCenterY); // Determined in paint program. Plane rotates around the center of the fuselage.
  planeSpr.setSwapBytes(true);
  planeSpr.pushImage(0, 0, planeOutlineWidth, planeOutlineHeight, planeOutline);

  ballSpr.createSprite(ballWidth, ballHeight);
  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball_image);


  Serial.println("\r\nInitialisation done.\r\n");
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

    static int i = 0;

    // For testing, we will just set things to values that excersize the elements.
    // In the final version, these will be set by Mobiflight.

    // Inclinometer ball goes from -1 to 1
    setInclinometerBall((i-50)*2.0/100.0);
    setTurnCoordNeedle(i);

    setApAltLight(i/10 % 2);
    setApHdLight(i/10 % 4);
    setApRdyLight(i<50);
    setApStLight(i>60);
    setApTrkLoLight(i>70);
    setApTrkHiLight(i<70);
    setApTrimUpLight(i>90);
    setApTrimDownLight(i<10);
    setLowVoltLight(!(i/10 % 5));
    

    if (i>100) i = 0; 
    else i++;

    // This part will be in the mobiflight event loop
    
    mainSpr.pushImage(0, 0, dialWidth, dialHeight, dial); // Put a fresh background in the changing area to overwrite old plane.
    displayBall();
    displayLeds();

    // Do the plane last. It's on top of all the others.
    displayTurnCoordNeedle();

    mainSpr.pushSprite(0, 0); // Push the main sprite to the screen

    delay(10);   // Wont need this in mobiflight; it handles delays.
}

void displayLeds()
{
  if (stLedState) mainSpr.pushImage(STDotX, STDotY, APDot_width, APDot_height, APDot);
  if (hdLedState) mainSpr.pushImage(HDDotX, HDDotY, APDot_width, APDot_height, APDot);
  if (trkLoLedState) mainSpr.pushImage(TrkLoDotX, TrkLoDotY, APDot_width, APDot_height, APDot);
  if (trkHiLedState) mainSpr.pushImage(TrkHiDotX, TrkHiDotY, APDot_width, APDot_height, APDot);

  if (altLedState) mainSpr.pushImage(AltDot_x, AltDot_y, AltDot_width, AltDot_height, AltDot);

  if (upLedState) mainSpr.pushImage(UpDot_x, UpDot_y, UpDot_width, UpDot_height, UpDot);
  if (downLedState) mainSpr.pushImage(DownDot_x, DownDot_y, DownDot_width, DownDot_height, DownDot);

  if (rdyLedState) mainSpr.pushImage(ReadyDot_x, ReadyDot_y, ReadyDot_width, ReadyDot_height, ReadyDot);

  if (lowVoltLedState) mainSpr.pushImage(LowVoltFlag_x, LowVoltFlag_y, LowVoltFlag_width, LowVoltFlag_height, LowVoltFlag);

  return;
}

// number from 0 to 100, with 50 being centered.
void setTurnCoordNeedle(double percent)
{
  turnCoorNeedle = percent;
}

// Number from -1 to 1, with 0 being centered.
void setInclinometerBall(double percent)
{
  inclinometerBall = percent;
}

void displayBall()
{ 
  double angle = inclinometerBall * 50;
  ballSpr.pushToSprite(&mainSpr, mainSpr.width() / 2 + angle - 13, mainSpr.height() - ballSpr.height() - round((abs(angle) / 5)) - 22, TFT_WHITE);
}

void displayTurnCoordNeedle()
{
  double angle = (turnCoorNeedle - 50) * 0.6;

  planeSpr.pushRotated( &mainSpr, (int16_t)angle, TFT_WHITE); // Push the plane sprite on the background

}

void setApTrimUpLight(bool state)
{
  upLedState = state;
}
void setApTrimDownLight(bool state)
{
  downLedState = state;
}
void setApTrkLoLight(bool state)
{
  trkLoLedState = state;
}
void setApTrkHiLight(bool state)
{
  trkHiLedState = state;
}
void setApStLight(bool state)
{
  stLedState = state;
}
void setApRdyLight(bool state)
{
  rdyLedState = state;
}
void setApHdLight(bool state)
{
  hdLedState = state;
}
void setApAltLight(bool state)
{
  altLedState = state;
}
void setLowVoltLight(bool state)
{
  lowVoltLedState = state;
}