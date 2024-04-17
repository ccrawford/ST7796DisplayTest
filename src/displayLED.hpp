#pragma once

// A class to hold the display info for the LEDs on the main display. 
// Includes image data, size, location, and current state.
#include <Arduino.h>
#include <TFT_eSPI.h>

class DisplayLED
{
public:
    DisplayLED();
    DisplayLED(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t * imageData);
    ~DisplayLED();

    void draw(TFT_eSprite &tft);
    void setState(bool state);
    bool getState();

private:
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    const uint16_t * imageData;
    bool state;
};

DisplayLED::DisplayLED()
{
    x = 0;
    y = 0;
    width = 0;
    height = 0;
    imageData = nullptr;
    state = false;
}
DisplayLED::DisplayLED(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t * imageData)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->imageData = imageData;
    state = false;
}

void DisplayLED::setState(bool state)
{
    this->state = state;
}

bool DisplayLED::getState()
{
    return state;
}

void DisplayLED::draw(TFT_eSprite &tft)
{
    if (state)
    {
        tft.pushImage(x, y, width, height, imageData);
    }
}



