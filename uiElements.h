#pragma once

#include <Arduino.h>
#include "ui.h"

#define BUTTON_BG_COLOR RA8875_LIGHT_GREY
#define BUTTON_BORDER_COLOR RA8875_BLACK

typedef void (*buttonCallback)(uint8_t);

struct Geo
{
  String    label;
  uint16_t  xPos;
  uint16_t  yPos;
  uint16_t  width;
  uint16_t  height;
  uint16_t  color1;
  uint16_t  color2;
};

class Button 
{
  private:
    Geo     _geo;
    bool    _latch = true;
    bool    _state = false;

  public:
    Button();
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    buttonCallback cb;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(bool state);
};

class Container
{
  private:

  public:
    Geo   geo;
    Container(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    virtual bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw();
};

class Header :  public Container
{
  using Container :: Container;
  private:

  public:
};

class Arrangement :  public Container
{
  using Container :: Container;
  private:

  public:
};

class PatternView :  public Container
{
  using Container::Container;
  private:

  public:
};

class TrackDetails :  public Container
{
  using Container :: Container;
  private:

  public:
};

class Controls :  public Container
{
  using Container :: Container;
  private:

  public:
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    Button button_start;
    Button button_continue;
    void layout();
};
