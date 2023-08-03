#pragma once

#include <Arduino.h>
#include "ui.h"
#include "x_globals.h"
#include "track.h"

// --- COLOR SCHEME ---

// Note: using two layers the color depth is 8-bit
// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// R2 R1 R0 -- -- G2 G1 G0 -- -- -- B1 B0 -- -- --

#define CM2K_PRIMARYRED       0xe000
#define CM2K_PRIMARYGREEN     0x0700
#define CM2K_PRIMARYBLUE      0x0018

#define CM2K_DARKRED          0xa100

#define CM2K_LIGHTGREY        0xc618 //0xef5d
#define CM2K_MEDIUMGREY       0xce99
#define CM2K_DARKGREY         0x7bef
#define CM2K_DARKBLUE         0x1175
#define CM2K_NAVY             0x3b35
#define CM2K_BABYBLUE         0xCEFC
#define CM2K_PURPLEBLUE       0xA55B
#define CM2K_GREYBLUE         0xadb9
#define CM2K_DARKORANGE       0xfda0
#define CM2K_PURPLE           0xf81f

// Top and right sections
#define MAIN_BG_COLOR           CM2K_GREYBLUE

// Header deco elements, bar, mute, stat etc
#define DECO_TOPROW_FILL_COLOR  CM2K_DARKBLUE
#define DECO_TOPROW_TEXT_COLOR  RA8875_WHITE

// Button colors
#define BUTTON_BG_COLOR         RA8875_WHITE
#define BUTTON_BORDER_COLOR     CM2K_DARKBLUE
#define BUTTON_SHADOW_COLOR     CM2K_DARKGREY
#define BUTTON_FILL_NORMAL      CM2K_BABYBLUE
#define BUTTON_FILL_PRESSED     CM2K_PURPLEBLUE 
#define BUTTON_TEXT_COLOR       CM2K_DARKBLUE

// Borders for sections that draw it
#define CONTAINER_BORDER_COLOR  BUTTON_BORDER_COLOR

// Indicator colors
#define INDICATOR_BG_COLOR      RA8875_WHITE
#define INDICATOR_BORDER_COLOR  CM2K_DARKBLUE
#define INDICATOR_TEXT_COLOR    CM2K_DARKBLUE
#define INDICATOR_LABEL_COLOR   CM2K_DARKBLUE

// Activity indicator for tracks
#define ACTIVITY_BG_COLOR       RA8875_WHITE
#define ACTIVITY_FILL_COLOR     CM2K_PURPLEBLUE

// Track colors
#define TRACK_SELECTED_COLOR    CM2K_DARKORANGE
#define TRACK_NORMAL_COLOR      RA8875_WHITE
#define TRACK_TEXT_COLOR        CM2K_DARKBLUE
#define TRACK_SELECTED_TEXT_COLOR   CM2K_DARKBLUE

// Arrangement colors
#define ARRITEM_SELECTED_COLOR      TRACK_SELECTED_COLOR
#define ARRITEM_NORMAL_COLOR        TRACK_NORMAL_COLOR
#define ARRITEM_TEXT_COLOR          TRACK_TEXT_COLOR
#define ARRITEM_SELECTED_TEXT_COLOR TRACK_SELECTED_TEXT_COLOR

// Editor colors
#define EDITOR_BG_COLOR             RA8875_WHITE
#define EDITOR_BORDER_COLOR         CM2K_NAVY
#define EDITOR_BLACKKEY_COLOR       CM2K_MEDIUMGREY  
#define EDITOR_NOTE_COLOR_DEFAULT   CM2K_NAVY
#define EDITOR_NOTE_COLOR_SELECTED  CM2K_PURPLE

#define EDITOR_GRID_MINOR_COLOR     CM2K_LIGHTGREY 
#define EDITOR_GRID_MAJOR_COLOR     CM2K_MEDIUMGREY

#define TEXTEDITOR_BG_COLOR       CM2K_PURPLEBLUE
#define TEXTEDITOR_BORDER_COLOR   CM2K_DARKBLUE

// -------

#define TYPE_BUTTON   0
#define TYPE_CHECKBOX 1

#define EDIT_TOOL_DELETE 0
#define EDIT_TOOL_ADD    1
#define EDIT_TOOL_MOVE   2

#define INDICATOR_LABEL_NONE    0
#define INDICATOR_LABEL_TOP     1
#define INDICATOR_LABEL_LEFT40  2
#define INDICATOR_LABEL_LEFT80  3
#define INDICATOR_LABEL_CUSTOM  4

#define TRANSPORT_W 110
#define CONTAINER_TITLE_H 20

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
    uint16_t textColor_off = BUTTON_TEXT_COLOR;
    uint16_t textColor_on = BUTTON_TEXT_COLOR;
    uint8_t type = TYPE_BUTTON;
    void set(bool newState);
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor);
    buttonCallback cb;
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType); 
    void draw(bool newState);
    void setLabelOffset(int xOffset, int yOffset);
    void setTextColors(uint16_t colorOff, uint16_t colorOn);
};

class Indicator
{
  private:
    Geo     _geo;
    void    _drawCommon();
    uint8_t _labelPosition;
    int _labelXoffset = 0;
    int _labelYoffset = 0;
    int _variableXoffset = 5;
    int _variableYoffset = 2;
 
  public:
    Indicator();
    buttonCallback cb;
    void layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2, uint8_t labelPosition);
    void setLabelOffset(int xOffset, int yOffset);
    void setVariableOffset(int xOffset, int yOffset);
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
    char trackName[13];
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
    //Button button_new;
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
    const String portStrings[NR_PORTS] = {"SER1", "USB1", "USB2", "USB3"};
    const String quantizeStrings[NR_QUANTIZESTEPS] = {"OFF ", "1/4 " ,"1/8 ", "1/16", "1/32", "1/3 "};
    const uint16_t quantizeSettings[NR_QUANTIZESTEPS] = {1, 192, 96, 48, 24, 64};
    const String compressStrings[NR_COMPRESSTEPS] = {"OFF", "1:2" ,"1:4", "1:8", "fix"};
    const uint16_t compressSettings[NR_COMPRESSTEPS] = {0, 1, 2, 3, 4};  // 63 + abs(velocity - 64) / C
    const String lengthStrings[NR_QUANTIZESTEPS] = {"OFF ", "1/4 " ,"1/8 ", "1/16", "1/32", "1/3 "};
    const uint16_t lengthSettings[NR_QUANTIZESTEPS] = {0, 192, 96, 48, 24, 64};
    
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
    void update(uint8_t portIndex, uint8_t trackNr, uint8_t channel, uint8_t quantizeIndex, int transpose, uint8_t loop, int velocity, uint8_t compressIndex,  uint8_t lengthIndex);
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

class TextEditor : public Container
{
  using Container :: Container;
  private:
    uint8_t _textPosition = 0;
    char _textBuffer[16];

  public:
    uint8_t callerViewID = 0;
    uint8_t stringLength = 12;
    Indicator indicator_text;
    
    Button button_characters[25];
    Button button_digits[10];
    Button button_backspace;

    Button button_ok;
    Button button_cancel;
    char * textVariable = nullptr;
    bool * flagVariable = nullptr;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw();
    void animate();
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
    Button button_new;
    Button button_exit;
    Button button_load;
    Button button_save;
    Button button_loadPatterns;
    Scrollbar scrollbar;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};

class NoteElement
{
  public:
    uint16_t eventIndex_noteOn = 0;
    uint16_t eventIndex_noteOff = 0;
    uint16_t xPos = 0;
    uint16_t yPos = 0;
    uint16_t width = 16;
    uint16_t height = 8;
    void draw(bool selected);
    void reDraw();
    bool checkCursor(uint16_t _xPos, uint16_t _yPos, uint8_t clickType);
};

class Grid
{
  private:
    Geo _geo;
    uint16_t _timestampToXpos(uint32_t timestamp);
    uint16_t _timestampToWidth(uint32_t timestamp);
    uint16_t _noteToYpos(uint8_t note);
    uint8_t   _yPosToNote(uint16_t yPos);
    uint32_t  _xPosToQuantizedTimestamp(uint16_t xPos);

  public:
    Track * track = nullptr;
    NoteElement noteElements[NR_EDITOR_NOTES];
    uint16_t firstNoteIndex = 36; // C2
    uint16_t lastNoteIndex = 60; // C4
    uint32_t firstTick = 0;
    uint32_t lastTick = RESOLUTION * 4; // 4 beats
    uint16_t nrVisibleEvents = 0;

    uint16_t nrRows = 25;
    uint16_t nrColumns  = 16; // 16 * 1/16th

    uint16_t gridSizePixels_x = 0; 
    uint16_t gridSizePixels_y = 0;
    uint16_t actualHeight = 0;
    uint16_t actualWidth = 0;

    int16_t selectedNoteId = -1;

    uint16_t color = 0x9CF3;
    void layout(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void draw();
    void clear();
    void drawNotes();
    void syncToTrack();
    bool checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void moveY(bool isUp);
    void moveX(bool isRight);
    void zoomY(bool isIn);
    void zoomX(bool isIn);

    void deleteNote(uint16_t noteOnEventIndex);
    void addNote(uint8_t note, uint32_t timestampOn, uint32_t timestampOff);

    void animate();
    void drawPosition(uint32_t timestamp);
    void clearPosition();
};

class GraphicEditor : public Container
{
  using Container :: Container;
  private:
    uint8_t _toolMode = EDIT_TOOL_DELETE;
  public:
    Track * track = nullptr;
    Grid grid;
    Button button_tool_delete;
    Button button_tool_add;
    
    Button button_gridMoveLeft;
    Button button_gridMoveRight;
    Button button_gridZoomX;
    
    Button button_gridMoveUp;
    Button button_gridMoveDown;
    Button button_gridZoomY;
    
    Indicator indicator_firstNote;
    Indicator indicator_lastNote;

    Indicator indicator_noteValue;
    Indicator indicator_noteOnTick;
    Indicator indicator_noteOffTick;
    
    Button button_exit;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
    void draw();
    //void drawNotes();
    void drawNoteInfo();
    void drawNoteRange();
    void setTrack(Track * _track);
    
};