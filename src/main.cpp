/*====================================================================================

  This example draws a jpeg image in a Sprite then plot a rotated copy of the Sprite
  to the TFT.

  The jpeg used in in the sketch Data folder (press Ctrl+K to see folder)

  The jpeg must be uploaded to the ESP8266 or ESP32 SPIFFS by using the Tools menu
  sketch data upload option of the Arduino IDE. If you do not have that option it can
  be added. Close the Serial Monitor window before uploading to avoid an error message!

  To add the upload option for the ESP8266 see:
  http://www.esp8266.com/viewtopic.php?f=32&t=10081
  https://github.com/esp8266/arduino-esp8266fs-plugin/releases

  To add the upload option for the ESP32 see:
  https://github.com/me-no-dev/arduino-esp32fs-plugin

  Created by Bodmer 6/1/19 as an example to the TFT_eSPI library:
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
#include "ball.h"
#include "plane_outline.h"
#include "over_write.h"
#include <FS.h>

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>                  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();             // Invoke custom library
TFT_eSprite spr = TFT_eSprite(&tft);   // Create Sprite object "spr" with pointer to "tft" object
TFT_eSprite bgSpr = TFT_eSprite(&tft); // Create Sprite object "spr" with pointer to "tft" object
TFT_eSprite ballBgSpr = TFT_eSprite(&tft); // Create Sprite object "spr" with pointer to "tft" object
TFT_eSprite ballSpr = TFT_eSprite(&tft); // Create Sprite object "spr" with pointer to "tft" object

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>          // Include the function extension library
TFT_eFEX fex = TFT_eFEX(&tft); // Create TFT_eFX object "fex" with pointer to "tft" object

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200); // Used for messages
  while (!Serial)
    ;          // Wait for Serial to be ready
  delay(1000); // wait for serial monitor to open
  Serial.println("\r\nRotated Sprite 3 example\n");
  tft.begin();
  tft.setRotation(0); // 0 & 2 Portrait. 1 & 3 landscape
  tft.setSwapBytes(true);
  bgSpr.setSwapBytes(true);
  ballBgSpr.setSwapBytes(true);
  //ballSpr.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

  LittleFSConfig cfg;
  cfg.setAutoFormat(false);
  LittleFS.setConfig(cfg);
  LittleFS.begin();
  // File f = LittleFS.open("/Eye_80x64.jpg", "r");

  // Create a sprite to hold the jpeg (or part of it)
  // CAC set impage depth.
  spr.setColorDepth(16);
  spr.createSprite(166, 33);
  bgSpr.createSprite(turnOverWriteWidth, turnOverWriteHeight);

  ballBgSpr.createSprite(ballBackgroundwidth, ballBackgroundHeight);
  ballSpr.createSprite(ballWidth, ballHeight);

  Serial.println("\r\nInitialisation done.\r\n");

  // Lists the files so you can see what is in the SPIFFS
  // fex.listSPIFFS();

  Dir dir = LittleFS.openDir("/");
  Serial.printf("File size: %d\n", dir.fileSize());
  while (dir.next())
  {
    Serial.print("File: ");
    Serial.print(dir.fileName());
    if (dir.fileSize())
    {
      File f = dir.openFile("r");
      Serial.println(f.size());
    }
  }

  // Note the / before the SPIFFS file name must be present, this means the file is in
  // the root directory of the SPIFFS, e.g. "/tiger.jpg" for a file called "tiger.jpg"

  // Send jpeg info to serial port
  fex.jpegInfo("/plane_outline.jpg");
  // fex.jpegInfo("/turn_coord_sprite.jpg");

  // Draw jpeg image in Sprite spr at 0,0
  // fex.drawJpeg("/turn_coord_sprite.jpg", 0 , 0, &spr);
  // fex.drawJpeg("/dial_background.jpg", 0 , 0, &spr);
  fex.drawBmp("/plane_outline.bmp", 0, 0, &spr);
  // fex.drawJpeg("/plane_outline.jpg", 0, 0, &spr);
  // tft.pushImage(0,0, planeBmpWidth, planeBmpHeight, plane_outline,  TFT_BLACK, cmap &spr);

  //bgSpr.pushImage(0, 0, turnOverWriteWidth, turnOverWriteHeight, turnOverwrite);
  //  tft.fillScreen(TFT_DARKGREY);
  //  fex.drawJpeg("/dial_background.jpg", 0 , 0);
  tft.pushImage(0, 0, dialWidth, dialHeight, dial);

  // Set the TFT pivot point to the centre of the screen
  //tft.setPivot(160, 148);

  bgSpr.setPivot(90, 45); // Center of the background.

  // Set Sprite pivot point to centre of Sprite
  spr.setPivot(spr.width() / 2, spr.height() / 2);
  ballBgSpr.setPivot(ballBgSpr.width()/2, 0);

  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball_image);
  
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{

  // Push Sprite to the TFT at 0,0 (not rotated)
  // CAC with black as transparent
  //  spr.pushSprite(0, 0, 0x00);

  //  delay(1000);

  // Push copies of Sprite rotated through increasing angles 0-360 degrees
  // with 45 degree increments
  for (int16_t angle = -30; angle <= 30; angle += 1)
  {
    bgSpr.pushImage(0, 0, turnOverWriteWidth, turnOverWriteHeight, turnOverwrite);   // Put a fresh background in the changing area to overwrite old plane.
    spr.pushRotated(&bgSpr, angle, TFT_BLACK);  // Push the plane sprite on the background
    bgSpr.pushSprite(69, 96);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    ballBgSpr.pushImage(0, 0, ballBackgroundwidth, ballBackgroundHeight, ball_background);   // Put a fresh background in the changing area to overwrite old plane.
    ballSpr.pushToSprite(&ballBgSpr, ballBgSpr.width()/2 + angle - 10, ballBgSpr.height() - ballSpr.height() - round((abs(angle)/5)) + 0, TFT_WHITE);  // Push the plane sprite on the background
    ballBgSpr.pushSprite(97,236);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!



    delay(50);
  }
  for (int16_t angle = 30; angle >= -30; angle -= 1)
  {
  bgSpr.pushImage(0, 0, turnOverWriteWidth, turnOverWriteHeight, turnOverwrite);
    spr.pushRotated(&bgSpr, angle, TFT_BLACK);
    bgSpr.pushSprite(69,96);

    ballBgSpr.pushImage(0, 0, ballBackgroundwidth, ballBackgroundHeight, ball_background);   // Put a fresh background in the changing area to overwrite old plane.
    ballSpr.pushToSprite(&ballBgSpr, ballBgSpr.width()/2 + angle - 10, ballBgSpr.height() - ballSpr.height() - round((abs(angle)/5)) + 0, TFT_WHITE);  // Push the plane sprite on the background
    ballBgSpr.pushSprite(97,236);  // Push the background with the plane to the screen at appropriate spot. Coicicental coordinates!

    delay(50);
  }

  delay(2000);

  // Move Sprite pivot to a point above the image at 40,-60
  // (Note: Top left corner is Sprite coordinate 0,0)
  // The TFT pivot point has already been set to middle of screen.
  /*                    .Pivot point at 40,-60
                        ^
                        |
                       -60
                  < 40 >|
                  ______V______
                 |             |
                 |   Sprite    |
                 |_____________|
  */
  /*
   spr.setPivot(40, -60);

   // Push Sprite to screen rotated about the new pivot points
   // negative angle rotates Sprite anticlockwise
   for (int16_t angle = 330; angle >= 0; angle -= 30) {
     spr.pushRotated(angle);
     yield(); // Stop watchdog triggering
   }

   delay(5000);
   */
}
//====================================================================================