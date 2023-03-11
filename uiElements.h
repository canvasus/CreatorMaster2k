#pragma once

#include <Arduino.h>
#include "ui.h"
#include "x_globals.h"

#define MAIN_BG_COLOR         0x9D39 //0xCEFC //light blue

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
#define INDICATOR_LABEL_NONE    0
#define INDICATOR_LABEL_TOP     1
#define INDICATOR_LABEL_LEFT40  2
#define INDICATOR_LABEL_LEFT80  3
#define INDICATOR_LABEL_CUSTOM  4

#define TRACK_SELECTED_COLOR    0xCEFC // dark blue
#define TRACK_NORMAL_COLOR      RA8875_WHITE

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

class Deco
{
  private:
    Geo     _geo;
    int     _textRelXposition = 0;
  public:
    Deco();
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t fillColor, uint16_t textColor, int textRelXposition);
    void draw();
};

class Button 
{
  private:
    Geo     _geo;
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
    void    _drawCommon();
    uint8_t _labelPosition;
    int _labelXoffset = 0;
    int _labelYoffset = 0;
  public:
    Indicator();
    buttonCallback cb;
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2, uint8_t labelPosition);
    void setLabelOffset(int xOffset, int yOffset);
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(uint16_t value);
    void draw(String string);
    void draw(uint16_t trp_bar, uint16_t trp_4th, uint16_t trp_16th, bool drawStatics);
};

class TrackRow
{
  private:
    Geo _geo;
  public:
    TrackRow();
    String trackName;
    uint8_t id;
    uint8_t channel;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    buttonCallback cb;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
    void activity();
    // activity indicator
};

class Container
{
  public:
    Geo   geo;
    Container(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    bool drawBorder = false;
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
  public:
    Indicator indicator_freeMem;
    Indicator indicator_transport;
    Indicator indicator_bpm;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class Arrangement :  public Container
{
  using Container::Container;
  public:
    Indicator indicator_patternLength;
    Indicator indicator_patternPosition;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class PatternView :  public Container
{
  using Container::Container;
  public:
    String patternName;
    Indicator indicator_patternName;
    TrackRow trackRows[NR_TRACKS];
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class TrackDetails :  public Container
{
  using Container::Container;
  private:
    const String quantizeStrings[6] = {"OFF  ", " 1/2", " 1/3", " 1/4" ," 1/8", "1/16"};
    const uint16_t quantizeSettings[6] = {0, (RESOLUTION * 2) - 1, (RESOLUTION * 3 / 4) - 1, RESOLUTION - 1, (RESOLUTION / 2) - 1, (RESOLUTION / 4) - 1};
  public:
    Indicator indicator_trackNr;
    Indicator indicator_quantize;
    Button button_clear;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
    void update(uint8_t trackNr, uint8_t quantize);
};

class Controls :  public Container
{
  using Container::Container;
  public:
    Button button_start;
    Button button_continue;
    Button button_stop;
    Button button_forward;
    Button button_fastForward;
    Button button_reverse;
    Button button_fastReverse;
    Button button_punch;
    Button button_record;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void layout();
};
