#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

HeaderView headerView = HeaderView("HEADER", 0, 0, SCREEN_XRES, HEADER_H, MAIN_BG_COLOR, RA8875_WHITE);
ArrangementView arrangementView = ArrangementView("ARRANGE", 0, HEADER_H + PADDING, ARRANGE_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
PatternView patternView = PatternView("PATTERN", ARRANGE_W + PADDING, HEADER_H + PADDING, PATTERN_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
TrackDetailsView trackDetailsView = TrackDetailsView("TRACK", ARRANGE_W + PATTERN_W + 2 * PADDING, HEADER_H + PADDING, TRACKDETAILS_W, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
ControlsView controlsView = ControlsView("CONTROL", ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 3 * PADDING, HEADER_H + PADDING, CONTROLS_W, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);
ListEditor listEditorView = ListEditor("LISTEDITOR", 0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);

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
elapsedMillis uiTimerSlow = 0;
const uint8_t uiSlowInterval = 100;
elapsedMillis uiResetTimer = 0;
const uint8_t uiResetInterval = 250;

uint8_t viewMode = VIEW_NORMAL;

void setupUI()
{
  Serial.print(F("UI SETUP..."));
  tft.begin(RA8875_800x480);

  tft.uploadUserChar(cursorOn, 0);
  tft.uploadUserChar(cursorOff, 1);
  tft.writeTo(L2);
  tft.clearScreen(MAIN_BG_COLOR);
  
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

  listEditorView.drawBorder = true;
  
  tft.writeTo(L1);
  tft.clearScreen(RA8875_MAGENTA);
  tft.setTransparentColor(RA8875_MAGENTA);
  tft.layerEffect(TRANSPARENT);
  Serial.println(F("DONE"));
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
    if (viewMode == VIEW_NORMAL) uiUpdateActivity();
    drawCursor();
  }
  if (uiTimerSlow > uiSlowInterval)
  {
    uiTimerSlow = 0;
    uiUpdateSlowItems();
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
      arrangementView.arrangementRows[arrItemIndex].startBars = arrangement.arrangementItems_a[arrItemIndex].startTick / transport.ticksPerBar;
      arrangementView.arrangementRows[arrItemIndex].active = (arrangement.arrangementItems_a[arrItemIndex].status == ARRITEM_ACTIVE);
      arrangementView.arrangementRows[arrItemIndex].draw(arrItemIndex == currentArrangementPosition);
    }
    else arrangementView.arrangementRows[arrItemIndex].clear();
  }
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    if (arrangement.arrangementItems_a[currentArrangementPosition].muteArray[trackId]) arrangementView.indicator_muteArray[trackId].draw("M");
    else arrangementView.indicator_muteArray[trackId].draw("-");
  }

  uint16_t currentLengthBeats = arrangement.arrangementItems_a[currentArrangementPosition].lengthTicks / transport.ticksPerBeat;
  uint16_t startBars = arrangement.arrangementItems_a[currentArrangementPosition].startTick  / transport.ticksPerBeat;
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
    //patternView.trackRows[trackId].trackName = patterns[currentPattern].tracks[trackId].name;
    memcpy(patternView.trackRows[trackId].trackName, patterns[currentPattern].tracks[trackId].name, 8);
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
  static elapsedMillis activityTimer = 0;
  const uint16_t screenSaverTimeout = 10000;
  static elapsedMillis debounceTimer = 0;
  const uint8_t debounceTime = 100;
  static uint8_t lastMouseButtons = 0;
   if (mouse1.available())
   {
    activityTimer = 0;
    int mouseX = mouse1.getMouseX();
    int mouseY = mouse1.getMouseY();
    if ( (abs(mouseX) > 0) || (abs(mouseY) > 0)) updateCursor(mouseX, mouseY);
   
    uint8_t mouseButtons = mouse1.getButtons();
    if (mouseButtons != lastMouseButtons)
    {
      activityTimer = 0;
      if (debounceTimer < debounceTime) mouseButtons = 0;
      else
      {
        debounceTimer = 0;
        lastMouseButtons = mouseButtons;
      }
      //if (mouseButtons != 0 && debounceTimer > debounceTime) debounceTimer = 0;
    }
    
    uint8_t mouseWheel = mouse1.getWheel();
    if (mouseWheel == 255 && mouseButtons == 0) mouseButtons = 2;
    if (mouseWheel == 1 && mouseButtons == 0) mouseButtons = 1;
    
    if (mouseButtons != 0 && viewMode == VIEW_NORMAL)
    {
      
      headerView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      arrangementView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      patternView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      trackDetailsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      controlsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
    }
    
    if (mouseButtons != 0 && viewMode == VIEW_LISTEDITOR)
    {
      headerView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      controlsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      listEditorView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
    }
    mouse1.mouseDataClear();
    
  }
  if (activityTimer > screenSaverTimeout) setScreensaver(true);
  else setScreensaver(false);
}

void setScreensaver(bool state)
{
  static bool screensaverState = false;
  if (state != screensaverState)
  {
    screensaverState = state;
    //tft.backlight(!screensaverState);
    //tft.sleep(screensaverState);
    Serial.printf("Screensaver: %d\n", screensaverState);
    //if (!state) delay(10);
//    tft.writeTo(L2);
//    if (screensaverState)
//    {
//      tft.clearScreen(0x1175);
//    }
//    else
//    {
//      tft.clearScreen(RA8875_MAGENTA);
//    }
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

void uiUpdateSlowItems()
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
}

void uiUpdateControls()
{
  // reset non-latching buttons etc
  if (controlsView.button_stop.state) controlsView.button_stop.set(false);
  if (controlsView.button_start.state) controlsView.button_start.set(false);
  if (trackDetailsView.button_copy.state) trackDetailsView.button_copy.set(false);
  if (trackDetailsView.button_paste.state) trackDetailsView.button_paste.set(false);
  if (trackDetailsView.button_clear.state) trackDetailsView.button_clear.set(false);
  if (arrangementView.button_new.state) arrangementView.button_new.set(false);
  if (arrangementView.button_delete.state) arrangementView.button_delete.set(false);
  //if (listEditorView.button_exit.state) listEditorView.button_exit.set(false);
  headerView.indicator_freeMem.draw(transport.freeMemory);
  if (headerView.button_load.state) headerView.button_load.set(false);
  if (headerView.button_save.state) headerView.button_save.set(false);
  if (headerView.button_new.state) headerView.button_new.set(false);
}

void recordClick(uint8_t clickType)
{
  static bool recordOn = false;
  String inUse = "in use";
  recordOn = !recordOn;
  record(recordOn);
  inUse.toCharArray(patterns[currentPattern].tracks[currentTrack].name, 8);
  memcpy(patternView.trackRows[currentTrack].trackName, patterns[currentPattern].tracks[currentTrack].name, 8);
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

void clearTrackClick(uint8_t clickType)
{
  String empty = "<empty>";
  clearTrack(currentTrack);
  empty.toCharArray(patterns[currentPattern].tracks[currentTrack].name, 8);
  empty.toCharArray(patternView.trackRows[currentTrack].trackName, 8);
  patternView.trackRows[currentTrack].draw(true);
}

void patternLengthClick(uint8_t clickType)
{
  uint16_t currentLengthBeats = arrangement.arrangementItems_a[currentArrangementPosition].lengthTicks / transport.ticksPerBeat;
  if (clickType == 1) currentLengthBeats++;
  if (clickType == 2 && currentLengthBeats > 0) currentLengthBeats--;
  arrangement.arrangementItems_a[currentArrangementPosition].lengthTicks = currentLengthBeats * transport.ticksPerBeat;
  arrangementView.indicator_patternLength.draw(currentLengthBeats / 4, currentLengthBeats % 4, 0, false);
  arrangement.updateArrangementStartPositions();
  uiRedrawArrangeView();
}

void patternSelectClick(uint8_t clickType)
{
  if (clickType == 1 && currentPattern < (NR_PATTERNS - 1) ) currentPattern++;
  if (clickType == 2 && currentPattern > 0) currentPattern--;
  arrangement.arrangementItems_a[currentArrangementPosition].patternIndex = currentPattern; // update arrangement data
  uiRedrawArrangeView(); // update arrangement view
}

void newArrangeItemClick(uint8_t clickType)
{
  uint8_t id = arrangement.newArrangementItem();
  arrangement.arrangementItems_a[id].patternIndex = currentPattern;
  arrangement.arrangementItems_a[id].lengthTicks = 4 * RESOLUTION;
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
  setMuteStatus();
}

void arrangementOnClick(uint8_t clickType)
{
  transport.arrangementOn = !transport.arrangementOn;
  if (transport.arrangementOn) headerView.indicator_arrOn.draw("ON");
  else headerView.indicator_arrOn.draw("OFF");
}

void muteArrayClick(uint8_t id)
{
  bool muteStatus = !arrangement.arrangementItems_a[currentArrangementPosition].muteArray[id];
  patterns[currentPattern].tracks[id].hidden = muteStatus;
  arrangement.arrangementItems_a[currentArrangementPosition].muteArray[id] = muteStatus;
  if (muteStatus) arrangementView.indicator_muteArray[id].draw("M");
  else arrangementView.indicator_muteArray[id].draw("-");
}

void signatureClick(uint8_t clickType)
{
  if (clickType == 1 && transport.signatureId < (NR_SIGNATURES - 1) ) setSignature(++transport.signatureId);
  if (clickType == 2 && transport.signatureId > 0) setSignature(--transport.signatureId);
  headerView.indicator_signature.draw(transport.signature);
}

void editTrackClick(uint8_t clickType)
{
  if (clickType == 1) uiSetListEditorViewMode();
}

void exitEditorClick(uint8_t clickType)
{
  if (clickType == 1) uiSetNormalViewMode();
}

void loadClick(uint8_t clickType)
{
  loadTrackEvents(currentPattern);
  uiRedrawPatternView();
  uiRedrawTrackDetailsView();
}

void saveClick(uint8_t clickType)
{
  saveTrackEvents(currentPattern);
}

void newClick(uint8_t clickType)
{
  clearArrangement();
  clearPatterns();
  uiRedrawArrangeView();
  uiRedrawPatternView();
  uiRedrawTrackDetailsView();
}

void copyTrackClick(uint8_t clickType) { copyTrack(); }

void pasteTrackClick(uint8_t clickType)
{
  String inUse = "in use";
  pasteTrack();
  inUse.toCharArray(patterns[currentPattern].tracks[currentTrack].name, 8);
  inUse.toCharArray(patternView.trackRows[currentTrack].trackName, 8);
  patternView.trackRows[currentTrack].draw(true);
}

void uiSetNormalViewMode()
{
  tft.writeTo(L2);
  tft.fillRect(0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  MAIN_BG_COLOR);
  // draw all normal elements
  viewMode = VIEW_NORMAL;
  
  arrangementView.draw();
  arrangementView.layout();

  setChannelIndicators();
  patternView.draw();
  patternView.layout();
  
  trackDetailsView.draw();
  trackDetailsView.layout();

  uiRedrawArrangeView();
  uiRedrawPatternView();
  uiRedrawTrackDetailsView();
}

void uiSetListEditorViewMode()
{
  tft.writeTo(L2);
  tft.fillRect(0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  MAIN_BG_COLOR);
  // replace arrangement and pattern view with list editor
  viewMode = VIEW_LISTEDITOR;
  
  listEditorView.draw();
  listEditorView.layout();
}

void testClick(uint8_t clickType)
{
  Serial.printf("Click type %d\n", clickType);
}

void testClickIndicator(uint8_t clickType)
{
  
}
