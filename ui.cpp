#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

Header header = Header("HEADER", 0, 0, SCREEN_XRES, HEADER_H, MAIN_BG_COLOR, RA8875_WHITE);
Arrangement arrangement = Arrangement("ARRANGE", 0, HEADER_H + PADDING, ARRANGE_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
PatternView patternView = PatternView("PATTERN", ARRANGE_W + PADDING, HEADER_H + PADDING, PATTERN_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
TrackDetails trackDetails = TrackDetails("TRACK", ARRANGE_W + PATTERN_W + 2 * PADDING, HEADER_H + PADDING, TRACKDETAILS_W, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
Controls controls = Controls("CONTROL", ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 3 * PADDING, HEADER_H + PADDING, CONTROLS_W, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);

uint16_t cursorXpos = 0;
uint16_t cursorYpos = 0;

uint16_t testVariable = 12345;

elapsedMillis uiTimer = 0;
const uint8_t uiInterval = 50;

void setupUI()
{
  tft.begin(RA8875_800x480);
  tft.writeTo(L2);
  tft.clearScreen(MAIN_BG_COLOR); //RA8875_GRAYSCALE * 20);
  header.draw();
  header.layout();
  arrangement.draw();
  patternView.draw();
  trackDetails.draw();
  controls.draw();
  controls.layout();
  
  tft.writeTo(L1);
  tft.clearScreen(RA8875_MAGENTA);
  tft.setTransparentColor(RA8875_MAGENTA);
  tft.layerEffect(TRANSPARENT);
}

void updateUI()
{
  updateMouse();
  if (uiTimer > uiInterval)
  {
    uiTimer = 0;
    uiUpdateTransport();
    uiUpdateControls();
    Serial.println(controls.button_start.state);
  }
}

void updateMouse()
{
  int mouseX = 0;
  int mouseY = 0;
   if (mouse1.available())
   {
    mouseX = mouse1.getMouseX();
    mouseY = mouse1.getMouseY();
    if ( (abs(mouseX) > 0) || (abs(mouseY) > 0)) updateCursor(mouseX, mouseY);
   
    uint8_t mouseButtons = mouse1.getButtons();
    if (mouseButtons != 0)
    {
      header.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      arrangement.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      patternView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      trackDetails.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      controls.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
    }
    mouse1.mouseDataClear();
  }
}

void updateCursor(int deltaX, int deltaY)
{
  static uint16_t oldCursorXpos = 0;
  static uint16_t oldCursorYpos = 0;
  cursorXpos = constrain(oldCursorXpos + deltaX, 10, SCREEN_XRES - 10);
  cursorYpos = constrain(oldCursorYpos + deltaY, 10, SCREEN_YRES - 10);;

  tft.writeTo(L1);
  tft.setCursor(oldCursorXpos, oldCursorYpos);
  tft.setTextColor(RA8875_MAGENTA);
  tft.print("X");

  tft.setCursor(cursorXpos, cursorYpos);
  tft.setTextColor(RA8875_RED);
  tft.print("X");
  oldCursorXpos = cursorXpos;
  oldCursorYpos = cursorYpos;
}

void uiUpdateTransport()
{
  header.indicator_transport.draw(transport.trp_bar + 1, transport.trp_4th + 1, transport.trp_16th + 1, transport.trp_768th + 1);
}

void uiUpdateControls()
{
  // reset non-latching buttons etc
  if (controls.button_stop.state && !controls.button_stop.latch) controls.button_stop.set(false);
  //if (controls.button_start.state && !controls.button_start.latch) controls.button_start.set(false);
}

void recordClick(uint8_t clickType)
{
  static bool recordOn = false;
  recordOn = !recordOn;
  record(recordOn);
}

void startClick(uint8_t clickType)
{
  if (controls.button_start.state) play();
  else stop();
}

void stopClick(uint8_t clickType)
{
  stop();
  reset();
  controls.button_start.set(false);
}

void testClick(uint8_t clickType)
{
  Serial.printf("Click type %d\n", clickType);
}

void testClickIndicator(uint8_t clickType)
{
  if (clickType == 1) header.indicator_freeMem.draw(++testVariable);
  if (clickType == 2) header.indicator_freeMem.draw(--testVariable);
}
