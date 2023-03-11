#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

Header header = Header("HEADER", 0, 0, SCREEN_XRES, HEADER_H, MAIN_BG_COLOR, RA8875_WHITE);
Arrangement arrangement = Arrangement("ARRANGE", 0, HEADER_H + PADDING, ARRANGE_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
PatternView patternView = PatternView("PATTERN", ARRANGE_W + PADDING, HEADER_H + PADDING, PATTERN_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
TrackDetails trackDetails = TrackDetails("TRACK", ARRANGE_W + PATTERN_W + 2 * PADDING, HEADER_H + PADDING, TRACKDETAILS_W, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
Controls controls = Controls("CONTROL", ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 3 * PADDING, HEADER_H + PADDING, CONTROLS_W, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);

const uint8_t cursorOn[16] = {
  0b00000000,
  0b11000000,
  0b11100000,
  0b11110000,
  0b11111000,
  0b11111100,
  0b11111110,
  0b11111111,
  0b11111110,
  0b11111100,
  0b11111100,
  0b11011100,
  0b10001110,
  0b00001110,
  0b00000111,
  0b00000000
};

const uint8_t cursorOff[16] = {0xFF, 0xFF, 0xFF, 0xFF,
                                0xFF, 0xFF, 0xFF, 0xFF,
                                0xFF, 0xFF, 0xFF, 0xFF,
                                0xFF, 0xFF, 0xFF, 0xFF};

uint16_t cursorXpos = 0;
uint16_t cursorYpos = 0;

uint16_t testVariable = 12345;

elapsedMillis uiTimer = 0;
const uint8_t uiInterval = 30;
elapsedMillis uiResetTimer = 0;
const uint8_t uiResetInterval = 250;


void setupUI()
{
  tft.begin(RA8875_800x480);
  tft.uploadUserChar(cursorOn, 0);
  tft.uploadUserChar(cursorOff, 1);
  tft.writeTo(L2);
  tft.clearScreen(MAIN_BG_COLOR); //RA8875_GRAYSCALE * 20);
  
  header.draw();
  header.layout();
  
  arrangement.drawBorder = true;
  arrangement.draw();
  arrangement.layout();
  
  patternView.drawBorder = true;
  patternView.draw();
  patternView.layout();
  
  trackDetails.drawBorder = true;
  trackDetails.draw();
  trackDetails.layout();
  
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
    drawCursor();
  }
  if (uiResetTimer > uiResetInterval)
  {
    uiResetTimer = 0;
    uiUpdateControls();
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
  cursorXpos = constrain(cursorXpos + deltaX, 10, SCREEN_XRES - 10);
  cursorYpos = constrain(cursorYpos + deltaY, 10, SCREEN_YRES - 10);;
}

void drawCursor()
{
  static uint16_t oldCursorXpos = 0;
  static uint16_t oldCursorYpos = 0;
  tft.writeTo(L1);
  tft.setCursor(oldCursorXpos, oldCursorYpos);
  //tft.setFontScale(1);
  tft.setTextColor(RA8875_MAGENTA);
  tft.showUserChar(0);
  //tft.print("^");

  tft.setCursor(cursorXpos, cursorYpos);
  tft.setTextColor(RA8875_RED);
  tft.showUserChar(0);
  //tft.print("^");
  //tft.setFontScale(0);
  oldCursorXpos = cursorXpos;
  oldCursorYpos = cursorYpos;
}

void uiUpdateTransport()
{
  header.indicator_transport.draw(transport.trp_bar + 1, transport.trp_4th + 1, transport.trp_16th + 1, false);

}

void uiUpdateControls()
{
  // reset non-latching buttons etc
  if (controls.button_stop.state && !controls.button_stop.latch) controls.button_stop.set(false);
  //if (controls.button_start.state && !controls.button_start.latch) controls.button_start.set(false);
  if (trackDetails.button_clear.state && !trackDetails.button_clear.latch) trackDetails.button_clear.set(false);
  header.indicator_freeMem.draw(transport.freeMemory);
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
  panic();
  controls.button_start.set(false);
}

void trackSelectClick(uint8_t id)
{
  patternView.trackRows[currentTrack].draw(false);
  currentTrack = id;
  patternView.trackRows[currentTrack].draw(true);
  trackDetails.update(currentTrack, getQuantize());
}

void bpmClick(uint8_t clickType)
{
  if (clickType == 1) setBpm(transport.bpm + 1);
  if (clickType == 2) setBpm(transport.bpm - 1);
  header.indicator_bpm.draw(transport.bpm);
}

void quantizeClick(uint8_t clickType)
{
  uint8_t q = getQuantize();
  if (clickType == 1)
  {
    if (q >= 192) q = 192;
    else q = q +1;
  }
  if (clickType == 2)
  {
    if (q == 0) q = 0;
    else q = q - 1;
  }
  setQuantize(q);
  trackDetails.indicator_quantize.draw(q);
}

void clearClick(uint8_t clickType) { clearTrack(currentTrack); }

void patternLengthClick(uint8_t clickType)
{
  uint16_t currentLengthBeats = patterns[currentPattern].lengthBeats;
  if (clickType == 1) currentLengthBeats++;
  if (clickType == 2 && currentLengthBeats > 0) currentLengthBeats--;
  patterns[currentPattern].lengthBeats = currentLengthBeats;
  arrangement.indicator_patternLength.draw(currentLengthBeats / 4 + 1, currentLengthBeats % 4, 0, false);
  Serial.printf("Length beats %d\n", currentLengthBeats);
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
