#pragma once

#include <SPI.h>
#include <RA8875.h>
#include <USBHost_t36.h>

#include "uiElements.h"
#include "sequencer.h"
#include "sdMgr.h"
#include "peripherals.h"

#define RA8875_CS 10   // ER-TFTM070 pin 5
#define RA8875_RESET 9 // ER-TFTM070 pin 11
#define RA8875_MOSI 11  //ER-TFTM070 pin 7
#define RA8875_MISO 12  //ER-TFTM070 pin 6
#define RA8875_SCLK 13   //ER-TFTM070 pin 8

//#define PADCONFIG ((1 << 0) | (1 << 3) | (1 << 6))

#define SCREEN_XRES 800
#define SCREEN_YRES 480
#define PADDING 6

#define ARRANGE_W       (uint16_t)(SCREEN_XRES * 0.32 - PADDING)
#define PATTERN_W       (uint16_t)(SCREEN_XRES * 0.32 - PADDING)
#define TRACKDETAILS_W  (uint16_t)(SCREEN_XRES * 0.16 - PADDING)
#define CONTROLS_W      (uint16_t)(SCREEN_XRES * 0.2)
#define HEADER_H        (uint16_t)(SCREEN_YRES * 0.1)
#define MAIN_H          (uint16_t)(SCREEN_YRES * 0.9 - PADDING)

#define MOUSE_LEFT    1
#define MOUSE_RIGHT   2
#define MOUSE_MIDDLE  4
#define MOUSE_WHL_UP    12
#define MOUSE_WHL_DOWN  11

#define VIEW_NORMAL      0
#define VIEW_EDITOR      1
#define VIEW_FILEMANAGER 2
#define VIEW_TEXTEDIT    3
#define VIEW_SYSTEM      4

#define TEXTEDIT_TRACK_NAME   0 
#define TEXTEDIT_PATTERN_NAME 1
#define TEXTEDIT_PROJECT_NAME 2

//extern KeyboardController keyboard1;
extern MouseController mouse1;
extern RA8875 tft;

void setupUI();
void setChannelIndicators();

void updateUI();
void updateMouse();
void updateCursor(int deltaX, int deltaY);
void drawCursor();

void recordClick(uint8_t clickType);
void startClick(uint8_t clickType);
void stopClick(uint8_t clickType);
void continueClick(uint8_t clickType);

void trackSelectClick(uint8_t id, uint8_t clickType);
void bpmClick(uint8_t clickType);
void portClick(uint8_t clickType);
void channelClick(uint8_t clickType);
void quantizeClick(uint8_t clickType);
void clearTrackClick(uint8_t clickType);
void editTrackClick(uint8_t clickType);
void transposeClick(uint8_t clickType);
void loopClick(uint8_t clickType);
void patternSelectClick(uint8_t clickType);

void newArrangeItemClick(uint8_t clickType);
void arrangementItemSelectClick(uint8_t id);
void deleteArrangeItemClick(uint8_t id);
void moveUpArrangeItemClick(uint8_t id);
void moveDownArrangeItemClick(uint8_t id);
void arrangementOnClick(uint8_t clickType);
void muteArrayClick(uint8_t id);
void patternLengthClick(uint8_t clickType);

void signatureClick(uint8_t clickType);
void exitEditorClick(uint8_t clickType);
void copyTrackClick(uint8_t clickType);
void pasteTrackClick(uint8_t clickType);
void velocityClick(uint8_t clickType);
void compressClick(uint8_t clickType);
void lengthClick(uint8_t clickType);
void leftLocatorClick(uint8_t clickType);
void rightLocatorClick(uint8_t clickType);
void cycleOnClick(uint8_t clickType);

void fileClick(uint8_t clickType);
void newClick(uint8_t clickType);
void loadClick(uint8_t clickType);
void loadPatternsClick(uint8_t clickType);
void saveClick(uint8_t clickType);
void fileManagerRowClick(uint8_t row, uint8_t clickType);

void systemClick(uint8_t clickType);

void scrollbarUpClick(uint8_t clickType);
void scrollbarDownClick(uint8_t clickType);

void editor_noteValueClick(uint8_t clickType);
void editor_noteOnTickClick(uint8_t clickType);
void editor_noteOffTickClick(uint8_t clickType);

void testClick(uint8_t clickType);
void testClickIndicator(uint8_t clickType);

void uiUpdateTransport();
void uiUpdateControls();
void uiUpdateActivity();
void uiUpdateSlowItems();

void uiRedrawArrangeView();
void uiRedrawPatternView();
void uiRedrawTrackDetailsView();
void uiRedrawHeaderView();

void uiSetNormalViewMode();
void uiSetEditorViewMode();
void uiSetFileManagerViewMode();
void uiSetTextEditViewMode();
void uiSetSystemViewMode();

void uiReturnFromTextEditor(bool status);

void setScreensaver(bool state);
