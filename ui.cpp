#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

HeaderView headerView = HeaderView("HEADER", 0, 0, SCREEN_XRES, HEADER_H, MAIN_BG_COLOR, RA8875_WHITE);
ArrangementView arrangementView = ArrangementView("ARRANGE", 0, HEADER_H + PADDING, ARRANGE_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
PatternView patternView = PatternView("PATTERN", ARRANGE_W + PADDING, HEADER_H + PADDING, PATTERN_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
TrackDetailsView trackDetailsView = TrackDetailsView("TRACK", ARRANGE_W + PATTERN_W + 2 * PADDING, HEADER_H + PADDING, TRACKDETAILS_W, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
ControlsView controlsView = ControlsView("CONTROL", ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 3 * PADDING, HEADER_H + PADDING, CONTROLS_W, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);

const uint8_t cursorOn[16] = {
  0b10000000,
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
  0b00000011
};

const uint8_t cursorOff[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint16_t cursorXpos = 0;
uint16_t cursorYpos = 0;

elapsedMillis uiTimer = 0;
const uint8_t uiInterval = 35;
elapsedMillis uiResetTimer = 0;
const uint8_t uiResetInterval = 250;

void setupUI()
{
  tft.begin(RA8875_800x480);
  tft.uploadUserChar(cursorOn, 0);
  tft.uploadUserChar(cursorOff, 1);
  tft.writeTo(L2);
  tft.clearScreen(MAIN_BG_COLOR); //RA8875_GRAYSCALE * 20);
  
  headerView.draw();
  headerView.layout();
  
  arrangementView.drawBorder = true;
  arrangementView.draw();
  arrangementView.layout();

  setChannelIndicators();
  patternView.drawBorder = true;
  patternView.draw();
  patternView.layout();
  
  trackDetailsView.drawBorder = true;
  trackDetailsView.draw();
  trackDetailsView.layout();
  
  controlsView.draw();
  controlsView.layout();
  
  tft.writeTo(L1);
  tft.clearScreen(RA8875_MAGENTA);
  tft.setTransparentColor(RA8875_MAGENTA);
  tft.layerEffect(TRANSPARENT);
}

void setChannelIndicators()
{
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    patternView.trackRows[trackId].channel = patterns[currentPattern].tracks[trackId].channel;
  }
}

void updateUI()
{
  updateMouse();
  if (uiTimer > uiInterval)
  {
    uiTimer = 0;
    uiUpdateTransport();
    uiUpdateActivity();
    drawCursor();
  }
  if (uiResetTimer > uiResetInterval)
  {
    uiResetTimer = 0;
    uiUpdateControls();
  }
}

void uiRedrawArrangeView()
{
  // redraw arrangement view (add, delete, reorder etc)
  for (uint8_t arrItemIndex = 0; arrItemIndex < NR_ARRITEMS; arrItemIndex++)
  {
    if (arrangement.arrangementItems_a[arrItemIndex].status == ARRITEM_ACTIVE)
    {
      uint8_t patternIndex = arrangement.arrangementItems_a[arrItemIndex].patternIndex;
      arrangementView.arrangementRows[arrItemIndex].patternName = patterns[patternIndex].name;
      arrangementView.arrangementRows[arrItemIndex].startBars = arrangement.arrangementItems_a[arrItemIndex].startBars;
      arrangementView.arrangementRows[arrItemIndex].active = (arrangement.arrangementItems_a[arrItemIndex].status == ARRITEM_ACTIVE);
      arrangementView.arrangementRows[arrItemIndex].draw(arrItemIndex == currentArrangementPosition);
    }
    else arrangementView.arrangementRows[arrItemIndex].clear();
  }

  uint16_t currentLengthBeats = patterns[currentPattern].lengthBeats;
  uint16_t startBars = arrangement.arrangementItems_a[currentArrangementPosition].startBars;
  arrangementView.indicator_patternLength.draw(currentLengthBeats / 4, currentLengthBeats % 4, 0, false);
  arrangementView.indicator_patternPosition.draw(startBars / 4, startBars % 4, 0, false);
}

void uiRedrawPatternView()
{
  // redraw pattern view using currentPattern data
  patternView.indicator_patternName.draw(patterns[currentPattern].name);
  currentTrack = 0;
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    patternView.trackRows[trackId].id = trackId;
    patternView.trackRows[trackId].trackName = patterns[currentPattern].tracks[trackId].name;
    patternView.trackRows[trackId].channel = patterns[currentPattern].tracks[trackId].channel;
    patternView.trackRows[trackId].draw(trackId == currentTrack);
  }
}

void uiRedrawTrackDetailsView()
{
  // redraw track detail view using currentPattern->currentTrack
  uint8_t channel = patterns[currentPattern].tracks[currentTrack].channel;
  uint8_t quantizeIndex = patterns[currentPattern].tracks[currentTrack].quantizeIndex;
  int transpose = patterns[currentPattern].tracks[currentTrack].transpose;
  uint8_t loop = patterns[currentPattern].tracks[currentTrack].loop;
  trackDetailsView.update(currentTrack, channel, quantizeIndex, transpose, loop);
}

void updateMouse()
{
  static elapsedMillis debounceTimer = 0;
  const uint8_t debounceTime = 50;
   if (mouse1.available())
   {
    int mouseX = mouse1.getMouseX();
    int mouseY = mouse1.getMouseY();
    if ( (abs(mouseX) > 0) || (abs(mouseY) > 0)) updateCursor(mouseX, mouseY);
   
    uint8_t mouseButtons = mouse1.getButtons();
    if (debounceTimer < debounceTime) mouseButtons = 0;
    if (mouseButtons != 0 && debounceTimer > debounceTime) debounceTimer = 0;
    
    uint8_t mouseWheel = mouse1.getWheel();
    if (mouseWheel == 255 && mouseButtons == 0) mouseButtons = 2;
    if (mouseWheel == 1 && mouseButtons == 0) mouseButtons = 1;
    
    if (mouseButtons != 0)
    {
      headerView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      arrangementView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      patternView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      trackDetailsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      controlsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
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
  tft.setTextColor(RA8875_MAGENTA, RA8875_MAGENTA);
  tft.showUserChar(1);
  tft.setCursor(cursorXpos, cursorYpos);
  tft.setTextColor(RA8875_RED, RA8875_MAGENTA);
  tft.showUserChar(0);
  oldCursorXpos = cursorXpos;
  oldCursorYpos = cursorYpos;
}

void uiUpdateTransport()
{
  headerView.indicator_transport.draw(transport.trp_bar + 1, transport.trp_4th + 1, transport.trp_16th + 1, false);
}

void uiUpdateActivity()
{
  static uint16_t smoothedActivity[NR_TRACKS];
  uint16_t recentActivity = 0;
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    recentActivity = patterns[currentPattern].getActivity(trackId);
    if (recentActivity > 0) smoothedActivity[trackId] = 10;
    if (recentActivity > 2) smoothedActivity[trackId] = 20;
    else if (smoothedActivity[trackId] > 0 ) smoothedActivity[trackId]--;
    patternView.trackRows[trackId].activity(smoothedActivity[trackId]);
  }
}

void uiUpdateControls()
{
  // react to arrangement/pattern/track index changes
  static uint8_t lastArrangementItem = 0;
  static uint8_t lastPatternIndex = 0;
  static uint8_t lastTrackIndex = 0;
  
  if (lastArrangementItem != currentArrangementPosition)
  {
    lastArrangementItem = currentArrangementPosition;
    uiRedrawArrangeView();
  }

  if (lastPatternIndex != currentPattern)
  {
    lastPatternIndex = currentPattern;
    uiRedrawPatternView();
    uiRedrawTrackDetailsView();
  }

  if ( (lastPatternIndex == currentPattern) && (lastTrackIndex != currentTrack) )
  {
    lastTrackIndex = currentTrack;
    uiRedrawTrackDetailsView();
  }
  
  // reset non-latching buttons etc
  if (controlsView.button_stop.state && !controlsView.button_stop.latch) controlsView.button_stop.set(false);
  if (trackDetailsView.button_clear.state && !trackDetailsView.button_clear.latch) trackDetailsView.button_clear.set(false);
  if (arrangementView.button_new.state) arrangementView.button_new.set(false);
  if (arrangementView.button_delete.state) arrangementView.button_delete.set(false);
  headerView.indicator_freeMem.draw(transport.freeMemory);
}

void recordClick(uint8_t clickType)
{
  static bool recordOn = false;
  recordOn = !recordOn;
  record(recordOn);
  patterns[currentPattern].tracks[currentTrack].name = "in use";
  patternView.trackRows[currentTrack].trackName = patterns[currentPattern].tracks[currentTrack].name;
  patternView.trackRows[currentTrack].draw(true);
}

void startClick(uint8_t clickType)
{
  if (controlsView.button_start.state) play();
  else stop();
}

void stopClick(uint8_t clickType)
{
  stop();
  reset();
  panic();
  controlsView.button_start.set(false);
}

void trackSelectClick(uint8_t id)
{
  patternView.trackRows[currentTrack].draw(false);
  currentTrack = id;
  patternView.trackRows[currentTrack].draw(true);
  //uiRedrawTrackDetailsView();
}

void bpmClick(uint8_t clickType)
{
  if (clickType == 1) setBpm(transport.bpm + 1);
  if (clickType == 2) setBpm(transport.bpm - 1);
  headerView.indicator_bpm.draw(transport.bpm);
}

void channelClick(uint8_t clickType)
{
  uint8_t channel = patterns[currentPattern].tracks[currentTrack].channel;
  if (clickType == 1 && channel < 16) channel++;
  if (clickType == 2 && channel > 0) channel--;
  trackDetailsView.indicator_channel.draw(channel);
  patterns[currentPattern].tracks[currentTrack].channel = channel;
  patternView.trackRows[currentTrack].channel = channel;
  patternView.trackRows[currentTrack].draw(true);
}

void quantizeClick(uint8_t clickType)
{
  uint8_t quantizeIndex = patterns[currentPattern].tracks[currentTrack].quantizeIndex;
  if (clickType == 1 && quantizeIndex < (NR_QUANTIZESTEPS - 1)) quantizeIndex++;
  if (clickType == 2 && quantizeIndex > 0) quantizeIndex--;
  trackDetailsView.indicator_quantize.draw(trackDetailsView.quantizeStrings[quantizeIndex]);
  patterns[currentPattern].tracks[currentTrack].quantizeIndex = quantizeIndex;
  patterns[currentPattern].tracks[currentTrack].quantize = trackDetailsView.quantizeSettings[quantizeIndex];
}

void transposeClick(uint8_t clickType)
{
  int transpose = patterns[currentPattern].tracks[currentTrack].transpose;
  if (clickType == 1 && transpose < 64) transpose++;
  if (clickType == 2 && transpose > -64) transpose--;
  patterns[currentPattern].tracks[currentTrack].transpose = transpose;
  trackDetailsView.indicator_transpose.draw(transpose);
}

void loopClick(uint8_t clickType)
{
  uint8_t loop = patterns[currentPattern].tracks[currentTrack].loop;
  if (clickType == 1 && loop < 64) loop++;
  if (clickType == 2 && loop > 0) loop--;
  patterns[currentPattern].tracks[currentTrack].loop = loop;
  trackDetailsView.indicator_loop.draw(loop);
}

void clearClick(uint8_t clickType)
{
  clearTrack(currentTrack);
  patterns[currentPattern].tracks[currentTrack].name = "<empty>";
  patternView.trackRows[currentTrack].trackName = patterns[currentPattern].tracks[currentTrack].name;
  patternView.trackRows[currentTrack].draw(true);
}

void patternLengthClick(uint8_t clickType)
{
  uint16_t currentLengthBeats = patterns[currentPattern].lengthBeats;
  if (clickType == 1) currentLengthBeats++;
  if (clickType == 2 && currentLengthBeats > 0) currentLengthBeats--;
  patterns[currentPattern].lengthBeats = currentLengthBeats;
  arrangement.arrangementItems_a[currentArrangementPosition].lengthBars = currentLengthBeats / 4;
  arrangementView.indicator_patternLength.draw(currentLengthBeats / 4, currentLengthBeats % 4, 0, false);
  Serial.printf("Pattern %d set to length %d beats %d bars\n", currentPattern, currentLengthBeats, currentLengthBeats/4);
}

void patternSelectClick(uint8_t clickType)
{
  if (clickType == 1 && currentPattern < (NR_PATTERNS - 1) ) currentPattern++;
  if (clickType == 2 && currentPattern > 0) currentPattern--;

  // update arrangement data
  arrangement.arrangementItems_a[currentArrangementPosition].patternIndex = currentPattern;
 
  // update arrangement view
  uiRedrawArrangeView();

  // update pattern view
  //uiRedrawPatternView();
  
  // update track detail view
  //uiRedrawTrackDetailsView();
}

void newArrangeItemClick(uint8_t clickType)
{
  uint8_t id = arrangement.newArrangementItem();
  arrangement.arrangementItems_a[id].patternIndex = currentPattern;
  arrangement.arrangementItems_a[id].lengthBars = patterns[currentPattern].lengthBeats / 4;
  uiRedrawArrangeView();
}

void deleteArrangeItemClick(uint8_t id)
{
  arrangement.deleteArrangementItem(id);
  if (currentArrangementPosition > 0) currentArrangementPosition--;
  uiRedrawArrangeView();
  uiRedrawPatternView();
  uiRedrawTrackDetailsView();
}

void arrangementItemSelectClick(uint8_t id)
{
  currentArrangementPosition = id;
  currentPattern = arrangement.arrangementItems_a[currentArrangementPosition].patternIndex;
  uiRedrawArrangeView();
  uiRedrawPatternView();
  uiRedrawTrackDetailsView();
}

void arrangementOnClick(uint8_t clickType)
{
  transport.arrangementOn = !transport.arrangementOn;
  if (transport.arrangementOn) headerView.indicator_arrOn.draw("ON");
  else headerView.indicator_arrOn.draw("OFF");
}



void testClick(uint8_t clickType)
{
  Serial.printf("Click type %d\n", clickType);
}

void testClickIndicator(uint8_t clickType)
{
  
}
