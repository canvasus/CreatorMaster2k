#pragma once

#include <SPI.h>
#include <RA8875.h>
#include <USBHost_t36.h>
#include "uiElements.h"
#include "sequencer.h"

#define RA8875_CS 10   // ER-TFTM070 pin 5
#define RA8875_RESET 9 // ER-TFTM070 pin 11
// MOSI=11,  ER-TFTM070 pin 7
// MISO=12,  ER-TFTM070 pin 6
// SCK=13,   ER-TFTM070 pin 8

#define SCREEN_XRES 800
#define SCREEN_YRES 480
#define PADDING 6

#define ARRANGE_W       (uint16_t)(SCREEN_XRES * 0.32 - PADDING)
#define PATTERN_W       (uint16_t)(SCREEN_XRES * 0.32 - PADDING)
#define TRACKDETAILS_W  (uint16_t)(SCREEN_XRES * 0.16 - PADDING)
#define CONTROLS_W      (uint16_t)(SCREEN_XRES * 0.2)
#define HEADER_H        (uint16_t)(SCREEN_YRES * 0.1)
#define MAIN_H          (uint16_t)(SCREEN_YRES * 0.9 - PADDING)

extern KeyboardController keyboard1;
extern MouseController mouse1;
extern RA8875 tft;

void setupUI();
void updateUI();
void updateMouse();
void updateCursor(int deltaX, int deltaY);
void drawCursor();

void recordClick(uint8_t clickType);
void startClick(uint8_t clickType);
void stopClick(uint8_t clickType);
void trackSelectClick(uint8_t id);
void bpmClick(uint8_t clickType);
void channelClick(uint8_t clickType);
void quantizeClick(uint8_t clickType);
void clearClick(uint8_t clickType);
void patternLengthClick(uint8_t clickType);
void transposeClick(uint8_t clickType);
void loopClick(uint8_t clickType);

void testClick(uint8_t clickType);
void testClickIndicator(uint8_t clickType);

void uiUpdateTransport();
void uiUpdateControls();
void uiUpdateActivity();
