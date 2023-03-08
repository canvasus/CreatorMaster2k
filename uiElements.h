#pragma once

#include <Arduino.h>
#include "ui.h"

#define MAIN_BG_COLOR         0xCEFC //light blue
#define BUTTON_BG_COLOR       RA8875_WHITE
#define BUTTON_BORDER_COLOR   0x1175 // dark blue
#define BUTTON_SHADOW_COLOR   0x7BEF // dark grey
#define BUTTON_FILL_NORMAL    0xCEFC // light blue
#define BUTTON_FILL_PRESSED   0xA55B // medium blue
#define BUTTON_TEXT_COLOR     0x1175 // dark blue

#define INDICATOR_BG_COLOR      RA8875_WHITE
#define INDICATOR_BORDER_COLOR  0x1175 // dark blue
#define INDICATOR_TEXT_COLOR    0x1175
#define INDICATOR_LABEL_COLOR   0x9CF3 // medium grey

#define CONTAINER_TITLE_COLOR 0xADB9
#define CONTAINER_TITLE_H 20

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
    //bool    _latch = true;
    //bool    _state = false;

  public:
    Button();
    bool state = false;
    bool latch = false;
    void set(bool newState);
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    buttonCallback cb;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(bool state);
};

class Indicator
{
  private:
    Geo     _geo;
    void _drawCommon();
  public:
    Indicator();
    buttonCallback cb;
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(uint16_t value);
    void draw(String string);
    void draw(uint16_t trp_bar, uint16_t trp_4th, uint16_t trp_16th, uint16_t trp_768th);
};

class TrackRow
{
  public:
    TrackRow();
  
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
    uint16_t relX(float fraction);
    uint16_t relY(float fraction);
    uint16_t relW(float fraction);
    uint16_t relH(float fraction);
};

class Header :  public Container
{
  using Container :: Container;
  private:

  public:
    Indicator indicator_freeMem;
    Indicator indicator_transport;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
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
    Button button_stop;
    Button button_forward;
    Button button_fastForward;
    Button button_reverse;
    Button button_fastReverse;
    Button button_punch;
    Button button_record;
    
    void layout();
};
