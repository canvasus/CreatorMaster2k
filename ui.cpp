#include "ui.h"

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

HeaderView headerView = HeaderView("HEADER", 0, 0, SCREEN_XRES, HEADER_H, MAIN_BG_COLOR, RA8875_WHITE);
ArrangementView arrangementView = ArrangementView("ARRANGE", 0, HEADER_H + PADDING, ARRANGE_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
PatternView patternView = PatternView("PATTERN", ARRANGE_W + PADDING, HEADER_H + PADDING, PATTERN_W, MAIN_H, RA8875_WHITE, RA8875_WHITE);
TrackDetailsView trackDetailsView = TrackDetailsView("TRACK", ARRANGE_W + PATTERN_W + 2 * PADDING, HEADER_H + PADDING, TRACKDETAILS_W, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
ControlsView controlsView = ControlsView("CONTROL", ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 3 * PADDING, HEADER_H + PADDING, CONTROLS_W, MAIN_H,  MAIN_BG_COLOR, RA8875_WHITE);
ListEditor listEditorView = ListEditor("LISTEDITOR", 0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  RA8875_WHITE, RA8875_WHITE);
FileManagerView fileManagerView = FileManagerView("FILEMANAGER", 0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  RA8875_WHITE, RA8875_WHITE);

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
const uint16_t uiResetInterval = 400;

uint8_t viewMode = VIEW_NORMAL;

const unsigned long screenSaverTimeout = 120000; // 2min

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
  fileManagerView.drawBorder = true;
  
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
    patternView.trackRows[trackId].channel = patterns[currentPattern].tracks[trackId].config.channel;
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

void uiRedrawHeaderView()
{
  headerView.indicator_signature.draw(transport.signature);
  headerView.indicator_bpm.draw(transport.bpm);
  if (transport.arrangementOn) headerView.indicator_arrOn.draw("ON");
  else headerView.indicator_arrOn.draw("OFF");
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
      arrangementView.arrangementRows[arrItemIndex].startBars = arrangement.arrangementItems_a[arrItemIndex].startTick / transport.ticksPerBar + 1;
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

  uint16_t lengthBars = arrangement.arrangementItems_a[currentArrangementPosition].lengthTicks / transport.ticksPerBar;
  uint16_t lengthBeats = (arrangement.arrangementItems_a[currentArrangementPosition].lengthTicks - lengthBars * transport.ticksPerBar)  / transport.ticksPerBeat;
  uint16_t startBars = arrangement.arrangementItems_a[currentArrangementPosition].startTick  / transport.ticksPerBar + 1;
  uint16_t startBeats = (arrangement.arrangementItems_a[currentArrangementPosition].startTick - (startBars - 1) * transport.ticksPerBar) / transport.ticksPerBeat;
  arrangementView.indicator_patternLength.draw(lengthBars, lengthBeats, 0, false);
  arrangementView.indicator_patternPosition.draw(startBars, startBeats, 0, false);
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
    memcpy(patternView.trackRows[trackId].trackName, patterns[currentPattern].tracks[trackId].config.name, 8);
    patternView.trackRows[trackId].channel = patterns[currentPattern].tracks[trackId].config.channel;
    patternView.trackRows[trackId].draw(trackId == currentTrack);
  }
}

void uiRedrawTrackDetailsView()
{
  // redraw track detail view using currentPattern->currentTrack
  uint8_t channel = patterns[currentPattern].tracks[currentTrack].config.channel;
  uint8_t quantizeIndex = patterns[currentPattern].tracks[currentTrack].config.quantizeIndex;
  int transpose = patterns[currentPattern].tracks[currentTrack].config.transpose;
  uint8_t loop = patterns[currentPattern].tracks[currentTrack].config.loop;
  int velocity = patterns[currentPattern].tracks[currentTrack].config.velocity;
  uint8_t compress = patterns[currentPattern].tracks[currentTrack].config.compressIndex;
  uint8_t length = patterns[currentPattern].tracks[currentTrack].config.lengthIndex;
  trackDetailsView.update(currentTrack, channel, quantizeIndex, transpose, loop, velocity, compress, length);
}

void updateMouse()
{
  static elapsedMillis activityTimer = 0;
  static elapsedMillis debounceTimer = 0;
  const uint8_t debounceTime = 250;
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
    }
    
    uint8_t mouseWheel = mouse1.getWheel();
    if (mouseWheel == 255 && mouseButtons == 0) mouseButtons = 2;
    if (mouseWheel == 1 && mouseButtons == 0) mouseButtons = 1;
    
    if (mouseButtons != 0)
    {
      headerView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      controlsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      
      if (viewMode == VIEW_NORMAL)
      {
        arrangementView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
        patternView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
        trackDetailsView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);        
      }

      if (viewMode == VIEW_LISTEDITOR) listEditorView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
      if (viewMode == VIEW_FILEMANAGER) fileManagerView.checkCursor(cursorXpos,  cursorYpos, mouseButtons);
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
    if (screensaverState)
    {
      tft.writeTo(L1);
      tft.backlight(!screensaverState);
      tft.clearScreen(0x7BEF);
    }
    else
    {
      tft.writeTo(L1);
      tft.backlight(!screensaverState);
      tft.clearScreen(RA8875_MAGENTA);
    }
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
  if (trackDetailsView.button_copy.state) trackDetailsView.button_copy.set(false);
  if (trackDetailsView.button_paste.state) trackDetailsView.button_paste.set(false);
  if (trackDetailsView.button_clear.state) trackDetailsView.button_clear.set(false);
  if (arrangementView.button_new.state) arrangementView.button_new.set(false);
  if (arrangementView.button_delete.state) arrangementView.button_delete.set(false);

  headerView.indicator_freeMem.draw(transport.freeMemory);
  if (headerView.button_file.state) headerView.button_file.set(false);
  if (headerView.button_new.state) headerView.button_new.set(false);
}

void recordClick(uint8_t clickType)
{
  static bool recordOn = false;
  const String inUse = "in use";
  if (clickType == 1)
  {
    recordOn = !recordOn;
    record(recordOn);
    inUse.toCharArray(patterns[currentPattern].tracks[currentTrack].config.name, 8);
    memcpy(patternView.trackRows[currentTrack].trackName, patterns[currentPattern].tracks[currentTrack].config.name, 8);
    patternView.trackRows[currentTrack].draw(true);
  }
}

void startClick(uint8_t clickType)
{
  if (clickType == 1)
  {
    play();
    controlsView.button_start.set(true);
    controlsView.button_stop.set(false);
  }
}

void stopClick(uint8_t clickType)
{
  if (clickType == 1)
  {
    stop();
    reset();
    panic();
    controlsView.button_start.set(false);
    controlsView.button_stop.set(true);
  }
}

void leftLocatorClick(uint8_t clickType)
{
  uint16_t barLeft = transport.leftLocatorTick / transport.ticksPerBar;
  uint16_t barRight = transport.rightLocatorTick / transport.ticksPerBar;
  if (clickType == 1 && (barLeft < (barRight - 1)) ) barLeft++;
  if (clickType == 2 && (barLeft > 0) ) barLeft--;
  transport.leftLocatorTick = barLeft * transport.ticksPerBar;
  controlsView.indicator_leftLocator.draw(barLeft + 1, 0, 0, false);
}

void rightLocatorClick(uint8_t clickType)
{
  uint16_t barLeft = transport.leftLocatorTick / transport.ticksPerBar;
  uint16_t barRight = transport.rightLocatorTick / transport.ticksPerBar;
  if (clickType == 1) barRight++;
  if (clickType == 2 && (barRight > (barLeft + 1)) ) barRight--;
  transport.rightLocatorTick = barRight * transport.ticksPerBar;
  controlsView.indicator_rightLocator.draw(barRight + 1, 0, 0, false);
}

void cycleOnClick(uint8_t clickType)
{
  transport.cycle = !transport.cycle;
  if (transport.cycle) controlsView.indicator_cycle.draw("ON");
  else controlsView.indicator_cycle.draw("OFF");
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
  uint8_t channel = patterns[currentPattern].tracks[currentTrack].config.channel;
  if (clickType == 1 && channel < 16) channel++;
  if (clickType == 2 && channel > 0) channel--;
  trackDetailsView.indicator_channel.draw(channel);
  patterns[currentPattern].tracks[currentTrack].config.channel = channel;
  patternView.trackRows[currentTrack].channel = channel;
  patternView.trackRows[currentTrack].draw(true);
}

void quantizeClick(uint8_t clickType)
{
  uint8_t quantizeIndex = patterns[currentPattern].tracks[currentTrack].config.quantizeIndex;
  if (clickType == 1 && quantizeIndex < (NR_QUANTIZESTEPS - 1)) quantizeIndex++;
  if (clickType == 2 && quantizeIndex > 0) quantizeIndex--;
  trackDetailsView.indicator_quantize.draw(trackDetailsView.quantizeStrings[quantizeIndex]);
  patterns[currentPattern].tracks[currentTrack].config.quantizeIndex = quantizeIndex;
  patterns[currentPattern].tracks[currentTrack].quantize = trackDetailsView.quantizeSettings[quantizeIndex];
}

void transposeClick(uint8_t clickType)
{
  int transpose = patterns[currentPattern].tracks[currentTrack].config.transpose;
  if (clickType == 1 && transpose < 64) transpose++;
  if (clickType == 2 && transpose > -64) transpose--;
  patterns[currentPattern].tracks[currentTrack].config.transpose = transpose;
  trackDetailsView.indicator_transpose.draw(transpose);
}

void loopClick(uint8_t clickType)
{
  uint8_t loop = patterns[currentPattern].tracks[currentTrack].config.loop;
  if (clickType == 1 && loop < 64) loop++;
  if (clickType == 2 && loop > 0) loop--;
  patterns[currentPattern].tracks[currentTrack].config.loop = loop;
  trackDetailsView.indicator_loop.draw(loop);
}

void velocityClick(uint8_t clickType)
{
  int velocity = patterns[currentPattern].tracks[currentTrack].config.velocity;
  if (clickType == 1 && velocity < 127) velocity++;
  if (clickType == 2 && velocity > -127) velocity--;
  patterns[currentPattern].tracks[currentTrack].config.velocity = velocity;
  trackDetailsView.indicator_velocity.draw(velocity);
}

void compressClick(uint8_t clickType)
{
  uint8_t compressIndex = patterns[currentPattern].tracks[currentTrack].config.compressIndex;
  if (clickType == 1 && compressIndex < (NR_COMPRESSTEPS - 1)) compressIndex++;
  if (clickType == 2 && compressIndex > 0) compressIndex--;
  trackDetailsView.indicator_compress.draw(trackDetailsView.compressStrings[compressIndex]);
  patterns[currentPattern].tracks[currentTrack].config.compressIndex = compressIndex;
  patterns[currentPattern].tracks[currentTrack].compress = trackDetailsView.compressSettings[compressIndex];
}

void lengthClick(uint8_t clickType)
{
  uint8_t lengthIndex = patterns[currentPattern].tracks[currentTrack].config.lengthIndex;
  if (clickType == 1 && lengthIndex < (NR_LENGTHSTEPS - 1)) lengthIndex++;
  if (clickType == 2 && lengthIndex > 0) lengthIndex--;
  trackDetailsView.indicator_length.draw(trackDetailsView.lengthStrings[lengthIndex]);
  patterns[currentPattern].tracks[currentTrack].config.lengthIndex = lengthIndex;
  patterns[currentPattern].tracks[currentTrack].length = trackDetailsView.lengthSettings[lengthIndex];
}

void clearTrackClick(uint8_t clickType)
{
  String empty = "<empty>";
  clearTrack(currentTrack);
  empty.toCharArray(patterns[currentPattern].tracks[currentTrack].config.name, 8);
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
  if (clickType == 1)
  {
    uint8_t id = arrangement.newArrangementItem();
    arrangement.arrangementItems_a[id].patternIndex = currentPattern;
    arrangement.arrangementItems_a[id].lengthTicks = 4 * RESOLUTION;
    uiRedrawArrangeView();
  }
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
  if (clickType == 1)
  {
    transport.arrangementOn = !transport.arrangementOn;
    if (transport.arrangementOn) headerView.indicator_arrOn.draw("ON");
    else headerView.indicator_arrOn.draw("OFF");
  }
}

void muteArrayClick(uint8_t id)
{
  bool muteStatus = !arrangement.arrangementItems_a[currentArrangementPosition].muteArray[id];
  patterns[currentPattern].tracks[id].config.hidden = muteStatus;
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

void editTrackClick(uint8_t clickType) { if (clickType == 1) uiSetListEditorViewMode(); }

void fileClick(uint8_t clickType) { uiSetFileManagerViewMode(); }

void exitEditorClick(uint8_t clickType) { if (clickType == 1) uiSetNormalViewMode(); }

void loadClick(uint8_t clickType)
{
  loadProject();
  uiSetNormalViewMode();
  uiRedrawHeaderView();
}

void saveClick(uint8_t clickType)
{
  saveProject();
  uiSetNormalViewMode();
}

void fileManagerRowClick(uint8_t id)
{
  fileManagerView.fileManagerRows[currentProject].draw(false);
  currentProject = id;
  fileManagerView.selectedIndex = id;
  setProjectfolder(id);
  fileManagerView.fileManagerRows[currentProject].draw(true);
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
  inUse.toCharArray(patterns[currentPattern].tracks[currentTrack].config.name, 8);
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
  uint16_t eventIndex = listEditorView.firstRowIndex;
  if (patterns[currentPattern].tracks[currentTrack].getNrEvents() > 0)
  {
    for (uint8_t listRow = 0; listRow < NR_LIST_ROWS; listRow++)
    {
      listEditorView.listEditorRows[listRow].draw(listRow == listEditorView.selectedIndex);
      eventIndex = eventIndex + listRow;
      eventIndex = min(eventIndex, patterns[currentPattern].tracks[currentTrack].getNrEvents() - 1);
      event tempEvent = patterns[currentPattern].tracks[currentTrack].events[eventIndex];
      listEditorView.listEditorRows[listRow].indicator_start_tick.draw(tempEvent.timestamp);
      listEditorView.listEditorRows[listRow].indicator_type.draw(tempEvent.type);
      listEditorView.listEditorRows[listRow].indicator_data1.draw(tempEvent.data1);
      listEditorView.listEditorRows[listRow].indicator_data2.draw(tempEvent.data2);
    }
  }
}

void uiSetFileManagerViewMode()
{
  tft.writeTo(L2);
  tft.fillRect(0, HEADER_H + PADDING, ARRANGE_W + PATTERN_W + TRACKDETAILS_W + 2 * PADDING, MAIN_H,  MAIN_BG_COLOR);
  // replace arrangement and pattern view with list editor
  viewMode = VIEW_FILEMANAGER;
  fileManagerView.draw();
  fileManagerView.layout();
}

void scrollbarUpClick(uint8_t clickType)
{
  
}

void scrollbarDownClick(uint8_t clickType)
{
  
}

void testClick(uint8_t clickType)
{
  Serial.printf("Click type %d\n", clickType);
}

void testClickIndicator(uint8_t clickType)
{
  
}
