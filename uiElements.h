#pragma once

#include <Arduino.h>
#include "ui.h"
#include "x_globals.h"

#define MAIN_BG_COLOR         0x9D39 //0xCEFC //light blue

#define DECO_TOPROW_FILL_COLOR  0x1175 // dark blue
#define DECO_TOPROW_TEXT_COLOR  RA8875_WHITE

#define BUTTON_BG_COLOR       RA8875_WHITE // button background color =  white
#define BUTTON_BORDER_COLOR   0x1175 // dark blue
#define BUTTON_SHADOW_COLOR   0x7BEF // dark grey
#define BUTTON_FILL_NORMAL    0xCEFC // light blue
#define BUTTON_FILL_PRESSED   0xA55B // medium blue
#define BUTTON_TEXT_COLOR     0x1175 // dark blue

#define TYPE_BUTTON   0
#define TYPE_CHECKBOX 1

#define INDICATOR_BG_COLOR      RA8875_WHITE
#define INDICATOR_BORDER_COLOR  0x1175 // dark blue
#define INDICATOR_TEXT_COLOR    0x1175
#define INDICATOR_LABEL_COLOR   0x1175 //0x9CF3 // medium grey
#define INDICATOR_LABEL_NONE    0
#define INDICATOR_LABEL_TOP     1
#define INDICATOR_LABEL_LEFT40  2
#define INDICATOR_LABEL_LEFT80  3
#define INDICATOR_LABEL_CUSTOM  4

#define ACTIVITY_BG_COLOR       RA8875_WHITE
#define ACTIVITY_FILL_COLOR     0xA55B

#define TRACK_SELECTED_COLOR    0x3B35 //0xCEFC // dark blue
#define TRACK_NORMAL_COLOR      RA8875_WHITE

#define CONTAINER_TITLE_COLOR 0xADB9
#define CONTAINER_TITLE_H 20

#define TRANSPORT_W 110

typedef void (*buttonCallback)(uint8_t);

class Geo
{
  public:
    Geo();
    void configure(String _label, uint16_t _xPos, uint16_t _yPos, uint16_t _width, uint16_t _height, uint16_t _color1, uint16_t _color2);
    uint16_t relX(float fraction);
    uint16_t relY(float fraction);
    uint16_t relW(float fraction);
    uint16_t relH(float fraction);
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
    int _labelXoffset = 10;
    int _labelYoffset = 10;
  public:
    Button();
    bool state = false;
    bool latch = false;
    bool drawBorder = true;
    uint8_t type = TYPE_BUTTON;
    void set(bool newState);
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    buttonCallback cb;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(bool state);
    void setLabelOffset(int xOffset, int yOffset);
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
    void draw(uint32_t value);
    void draw(int value);
    void draw(String string);
    void draw(uint16_t trp_bar, uint16_t trp_4th, uint16_t trp_16th, bool drawStatics);
};

class Scrollbar
{
  private:
    Geo      _geo;
    uint8_t  _orientation;
    
  public:
    Scrollbar();
    void configure(uint8_t orientation, uint16_t maxValue, uint16_t minValue, uint16_t inView);
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    uint16_t rangeInView;
    uint16_t max;
    uint16_t min;
    bool     isDragging;
    Indicator indicator_up;
    Indicator indicator_down;
    uint16_t position;
    void draw();
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType);
};

class TrackRow
{
  private:
    Geo _geo;
  public:
    TrackRow();
    char trackName[8];
    uint8_t id;
    uint8_t channel;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    buttonCallback cb;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
    void activity(uint8_t level);
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

class HeaderView : public Container
{
  using Container :: Container;
  public:
    Button button_file;
    Button button_new;
    Indicator indicator_freeMem;
    Indicator indicator_arrOn;
    Indicator indicator_transport;
    Indicator indicator_bpm;
    Indicator indicator_signature;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class ArrangementRow
{
  private:
    Geo _geo;
  public:
    ArrangementRow();
    uint8_t id;
    bool active;
    uint16_t startBars;
    String patternName;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    buttonCallback cb;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
    void clear();
};

class ArrangementView :  public Container
{
  using Container::Container;
  public:
    Deco decoLeft;
    Indicator indicator_arrange;
    Deco decoRight;
    ArrangementRow arrangementRows[NR_ARRITEMS];
    Button button_new;
    Button button_delete;
    Button button_up;
    Button button_down;
    Indicator indicator_muteArray[NR_TRACKS];
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
    Deco decoLeft;
    Deco decoRight;
    Indicator indicator_patternName;
    TrackRow trackRows[NR_TRACKS];
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class TrackDetailsView :  public Container
{
  using Container::Container;
  public:
    const String portStrings[NR_PORTS] = {"SER1"};
    const String quantizeStrings[NR_QUANTIZESTEPS] = {"OFF ", "1/4 " ,"1/8 ", "1/16", "1/3 "};
    const uint16_t quantizeSettings[NR_QUANTIZESTEPS] = {1, 192, 96, 48, 64};
    const String compressStrings[NR_COMPRESSTEPS] = {"OFF", "1:2" ,"1:4", "1:8", "fix"};
    const uint16_t compressSettings[NR_COMPRESSTEPS] = {0, 1, 2, 3, 4};  // 63 + abs(velocity - 64) / C
    const String lengthStrings[NR_QUANTIZESTEPS] = {"OFF ", "1/4 " ,"1/8 ", "1/16", "1/3 "};
    const uint16_t lengthSettings[NR_QUANTIZESTEPS] = {0, 192, 96, 48, 64};
    
    Deco decoLeft;
    Deco decoRight;
    Indicator indicator_trackNr;
    Indicator indicator_port;
    Indicator indicator_channel;
    Indicator indicator_quantize;
    Indicator indicator_transpose;
    Indicator indicator_loop;
    Indicator indicator_velocity;
    Indicator indicator_compress;
    Indicator indicator_length;
    Button button_copy;
    Button button_paste;
    Button button_edit;
    Button button_clear;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
    void update(uint8_t trackNr, uint8_t channel, uint8_t quantizeIndex, int transpose, uint8_t loop, int velocity, uint8_t compressIndex,  uint8_t lengthIndex);
};

class ControlsView :  public Container
{
  using Container::Container;
  public:
    Indicator indicator_leftLocator;
    Indicator indicator_rightLocator;
    Indicator indicator_cycle;
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

class ListEditorRow
{
  private:
    Geo     _geo;
  public:
    ListEditorRow();
    uint8_t id;
    Indicator indicator_start_tick;
    Indicator indicator_start_bar;
    Indicator indicator_start_4th;
    Indicator indicator_start_16th;
    Indicator indicator_start_768th; // note: this is one tick with RESOLUTION = 192
    Indicator indicator_type;
    Indicator indicator_channel;
    Indicator indicator_data1;
    Indicator indicator_data2;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
    void draw(bool selected, uint32_t startTick, uint8_t type, uint8_t data1, uint8_t data2);
};

class ListEditor : public Container
{
  using Container :: Container;
  private:
    Geo     _geo;  
  public:
    uint16_t firstRowIndex = 0;
    uint16_t selectedIndex = 0;
    Indicator indicator_header;
    ListEditorRow listEditorRows[NR_LIST_ROWS];
    Scrollbar scrollbar;
    Button button_exit;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class OnscreenKeyboard// : public Container
{
  //using Container :: Container;
  private:
    Geo _geo;
  public:
    Indicator letters[26];
    Button digits[10];
    Button button_ok;
    Button button_cancel;
    Button button_shift;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
};

class FileManagerRow
{
  private:
    Geo     _geo;  
  public:
    FileManagerRow();
    uint8_t id;
    bool isActive;
    Indicator indicator_name;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    
};

class FileManagerView : public Container
{
  using Container :: Container;
  private:
    Geo     _geo;  
  public:
    uint16_t firstRowIndex = 0;
    uint16_t selectedIndex = 0;
    FileManagerRow fileManagerRows[NR_FILE_ROWS];
    Button button_exit;
    Button button_load;
    Button button_save;
    Button button_loadPatterns;
    Scrollbar scrollbar;
    OnscreenKeyboard keyboard;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

