#include "uiElements.h"

// --- DECO CLASS ---
Deco::Deco() { }

void Deco::layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t fillColor, uint16_t textColor, int textRelXposition)
{
  _geo.label = label;
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = fillColor;
  _geo.color2 = textColor;
  _textRelXposition = textRelXposition;
}

void Deco::draw()
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, _geo.color1);
  tft.setTextColor(_geo.color2);
  tft.setCursor(_geo.xPos + _textRelXposition , _geo.yPos + 2);
  tft.print(_geo.label);
}

// --- CONTAINER CLASS ---
Container::Container(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t bgColor, uint16_t selectedColor)
{
  geo.label = label;
  geo.xPos = xPos;
  geo.yPos = yPos;
  geo.width = width;
  geo.height = height;
  geo.color1 = bgColor;
  geo.color2 = selectedColor;
}

void Container::draw()
{
  tft.writeTo(L2);
  tft.fillRect(geo.xPos, geo.yPos , geo.width, geo.height, geo.color1);
  if (drawBorder) tft.drawRect(geo.xPos, geo.yPos , geo.width, geo.height, BUTTON_BORDER_COLOR);
}

bool Container::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < geo.xPos) || (xPos > geo.xPos + geo.width) || (yPos < geo.yPos) || (yPos > geo.yPos + geo.height) ) return false;
  else return checkChildren(xPos, yPos, clickType);
}

bool Container::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType) { return false; } // dummy function overloaded by inheriting classes

uint16_t Container::relX(float fraction) { return (uint16_t)(geo.xPos + geo.width * fraction); }
uint16_t Container::relY(float fraction) { return (uint16_t)(geo.yPos + geo.height * fraction); }
uint16_t Container::relW(float fraction) { return (uint16_t)(geo.width * fraction); }
uint16_t Container::relH(float fraction) { return (uint16_t)(geo.height * fraction); }

// --- BUTTON CLASS ---
Button::Button(){ cb = nullptr; }

void Button::layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.label = label;
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void Button::draw(bool state)
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BG_COLOR); // background
  tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BORDER_COLOR); // border
  if (!state) //not pressed
  {
    tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 11, _geo.height - 11, BUTTON_SHADOW_COLOR);  //shadow
    tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color1); // main button area (state off)
    tft.drawRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, BUTTON_BORDER_COLOR); // main button border
  }
  else // pressed
  {
    tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 11, _geo.height - 11, BUTTON_BG_COLOR);  // clear shadow
    tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color2); // main button area (state on)
    tft.drawRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, BUTTON_BORDER_COLOR); // main button border
  }

  tft.setTextColor(BUTTON_TEXT_COLOR);
  tft.setCursor(_geo.xPos + 10, _geo.yPos + 10);
  tft.print(_geo.label);
}

bool Button::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    state = !state;
    if (cb) cb(clickType);
    draw(state);
    return true;
  }
}

void Button::set(bool newState)
{
  state = newState;
  draw(state);
}

// --- INDICATOR CLASS ---
Indicator::Indicator() { cb = nullptr; }

void Indicator::layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2, uint8_t labelPosition)
{
  _geo.label = label;
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
  _labelPosition = labelPosition;
}

void Indicator::setLabelOffset(int xOffset, int yOffset)
{
  _labelXoffset = xOffset;
  _labelYoffset = yOffset;
}

void Indicator::_drawCommon()
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BG_COLOR); // background
  tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BORDER_COLOR); // border
  if (_labelPosition != INDICATOR_LABEL_NONE)
  {
    if (_labelPosition == INDICATOR_LABEL_TOP) tft.setCursor(_geo.xPos, _geo.yPos - 16);
    if (_labelPosition == INDICATOR_LABEL_LEFT40) tft.setCursor(_geo.xPos - 40, _geo.yPos + 2);
    if (_labelPosition == INDICATOR_LABEL_LEFT80) tft.setCursor(_geo.xPos - 80, _geo.yPos + 2);
    if (_labelPosition == INDICATOR_LABEL_CUSTOM) tft.setCursor(_geo.xPos + _labelXoffset, _geo.yPos + _labelYoffset);
    tft.setTextColor(INDICATOR_LABEL_COLOR);
    tft.print(_geo.label);
  }
  tft.setCursor(_geo.xPos + 5, _geo.yPos + 2); // set position for main text
  tft.setTextColor(INDICATOR_TEXT_COLOR);
}

void Indicator::draw(int value)
{
  _drawCommon();
  tft.print(value);
}

void Indicator::draw(uint16_t value)
{
  _drawCommon();
  tft.print(value);
}

void Indicator::draw(String string)
{
  _drawCommon();
  tft.print(string);
}

void Indicator::draw(uint16_t trp_bar, uint16_t trp_4th, uint16_t trp_16th, bool drawStatics)
{
  if (drawStatics) _drawCommon();
  else tft.fillRect(_geo.xPos + 1, _geo.yPos + 1 , _geo.width - 2, _geo.height - 2, INDICATOR_BG_COLOR);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.setCursor(_geo.xPos + _geo.width * 0.05, _geo.yPos + 2);
  tft.print(trp_bar);
  tft.setCursor(_geo.xPos + _geo.width * 0.25, _geo.yPos + 2);
  tft.print(trp_4th);
  tft.setCursor(_geo.xPos + _geo.width * 0.50, _geo.yPos + 2);
  tft.print(trp_16th);
}

bool Indicator::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    if (cb) cb(clickType);
    return true;
  }
}

// --- TRACK ROW CLASS ---
TrackRow::TrackRow() { }

void TrackRow::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void TrackRow::draw(bool selected)
{
  tft.writeTo(L2);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("%2d", id);
  tft.setCursor(_geo.xPos + 50, _geo.yPos + 5);
  tft.print(trackName);
  tft.setCursor(_geo.xPos + _geo.width - 20, _geo.yPos + 5);
  tft.print(channel);
}

bool TrackRow::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    if (cb) cb(id);
    return true;
  }
}

void TrackRow::activity(uint8_t level)
{
  const uint8_t a_width = 18;
  level = constrain(level, 0, 20);
  tft.fillRect(_geo.xPos + 3, _geo.yPos + 1, a_width , _geo.height - 2, ACTIVITY_BG_COLOR);
  if (level > 0) tft.fillRect(_geo.xPos + 3, _geo.yPos + 3, (uint16_t)( (level/20.0) * a_width) , _geo.height - 6, ACTIVITY_FILL_COLOR);
}

// --- PATTERN VIEW CLASS ---

void PatternView::layout()
{
  indicator_patternName.layout("PATTERN", relX(0.2), relY(0), relW(0.6), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_NONE);
  indicator_patternName.draw("PATTERN01");
  
  decoLeft.layout("STAT", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 4);
  decoLeft.draw();
  
  decoRight.layout("CHN", relX(0.80), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 10);
  decoRight.draw();

  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    trackRows[trackId].cb = &trackSelectClick;
    trackRows[trackId].trackName = "test";
    trackRows[trackId].id = trackId;
    trackRows[trackId].layout(relX(0), relY((1/17.0) * (trackId + 1)), relW(1.0), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
    trackRows[trackId].draw(trackId == currentTrack);
  }
}

bool PatternView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  bool matchClick = false;
  uint8_t trackId = 0;
  while (!matchClick && trackId < NR_TRACKS)
  {
    trackRows[trackId++].checkCursor(xPos, yPos, clickType); 
  }
  return false;
}

// --- TRACK DETAILS CLASS ---

void TrackDetails::layout()
{
  decoLeft.layout("", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 0);
  decoLeft.draw();
  
  decoRight.layout("", relX(0.80), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 0);
  decoRight.draw();
  
  indicator_trackNr.layout("TRACK:", relX(0.6), relY(0), relW(0.2), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_trackNr.setLabelOffset(-48, 2);
  indicator_trackNr.draw(1);

  indicator_channel.layout("CHANNEL", relX(0.66), relY(2/17.0), relW(0.31), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_channel.cb = &channelClick;
  indicator_channel.setLabelOffset(-74, 2);
  indicator_channel.draw(1);
  
  indicator_quantize.layout("QUANTIZE", relX(0.66), relY(3/17.0), relW(0.31), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_quantize.cb = &quantizeClick;
  indicator_quantize.setLabelOffset(-74, 2);
  indicator_quantize.draw(quantizeStrings[0]);

  indicator_transpose.layout("TRANSPOSE", relX(0.66), relY(4/17.0), relW(0.31), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_transpose.cb = &transposeClick;
  indicator_transpose.setLabelOffset(-74, 2);
  indicator_transpose.draw(0);

  indicator_loop.layout("LOOP", relX(0.66), relY(5/17.0), relW(0.31), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_loop.cb = &loopClick;
  indicator_loop.setLabelOffset(-74, 2);
  indicator_loop.draw("");

  button_clear.layout("CLEAR", relX(0.1), relY(0.9), relW(0.6), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL);
  button_clear.cb = &clearClick;
  button_clear.draw(false);
}

bool TrackDetails::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_channel.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_quantize.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_transpose.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_loop.checkCursor(xPos, yPos, clickType)) return true;
  if (button_clear.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}

void TrackDetails::update(uint8_t trackNr, uint8_t channel, uint8_t quantizeIndex, int transpose, uint8_t loop)
{
  indicator_trackNr.draw(trackNr + 1);
  indicator_channel.draw(channel);
  indicator_quantize.draw(quantizeStrings[quantizeIndex]);
  indicator_transpose.draw(transpose);
  if (loop > 0) indicator_loop.draw(loop);
  else indicator_loop.draw(" ");
}

// --- CONTROLS ---

void Controls::layout()
{
  button_start.layout("START", relX(0.02), relY(0.9), relW(0.56), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_start.draw(false);
  button_start.cb = &startClick;
  
  button_continue.layout("CONT", relX(0.6), relY(0.9), relW(0.36),relH(0.1), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_continue.draw(false);
  button_continue.cb = &testClick;
  
  button_stop.layout("STOP", relX(0.6), relY(0.65), relW(0.36),relH(0.24), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_stop.draw(false);
  button_stop.cb = &stopClick;
  
  button_reverse.layout("<<", relX(0.02), relY(0.65), relW(0.27), relH(0.1), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_reverse.draw(false);
  button_reverse.cb = &testClick;
  
  button_forward.layout(">>", relX(0.30), relY(0.65), relW(0.27), relH(0.1), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_forward.draw(false);
  button_forward.cb = &testClick;
  
  button_fastReverse.layout("<<<", relX(0.02), relY(0.76), relW(0.27), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_fastReverse.draw(false);
  button_fastReverse.cb = &testClick;
  
  button_fastForward.layout(">>>", relX(0.30), relY(0.76), relW(0.27), relH(0.1), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_fastForward.draw(false);
  button_fastForward.cb = &testClick;

  button_punch.layout("PUNCH", relX(0.02), relY(0.50), relW(0.45), relH(0.1), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_punch.draw(false);
  button_punch.cb = &testClick;

  button_record.layout("RECORD", relX(0.52), relY(0.50), relW(0.45), relH(0.1), BUTTON_FILL_NORMAL, 0xFBCF);
  button_record.draw(false);
  button_record.cb = &recordClick;
}

bool Controls::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_start.checkCursor(xPos, yPos, clickType)) return true;
  if (button_continue.checkCursor(xPos, yPos, clickType)) return true;
  if (button_stop.checkCursor(xPos, yPos, clickType)) return true;
  if (button_reverse.checkCursor(xPos, yPos, clickType)) return true;
  if (button_forward.checkCursor(xPos, yPos, clickType)) return true;
  if (button_fastReverse.checkCursor(xPos, yPos, clickType)) return true;
  if (button_fastForward.checkCursor(xPos, yPos, clickType)) return true;
  if (button_punch.checkCursor(xPos, yPos, clickType)) return true;
  if (button_record.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}

// --- HEADER CLASS ---

void Header::layout()
{
  indicator_freeMem.layout("MEM", relX(0.01), relY(0.5), relW(0.1), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR,INDICATOR_LABEL_TOP);
  indicator_freeMem.draw(MEMORY_MAX);

  indicator_transport.layout("bar 4  16", relX(0.80), relY(0.5), relW(0.10), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_transport.draw(0,0,0, true);

  indicator_bpm.layout("BPM", relX(0.40), relY(0.5), relW(0.08), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_bpm.cb = &bpmClick;
  indicator_bpm.draw(transport.bpm);
}

bool Header::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_freeMem.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_bpm.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}

// --- ARRANGEMENT CLASS ---

void Arrangement::layout()
{
  decoLeft.layout("BAR", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, relW(0.05));
  decoLeft.draw();
  
  indicator_arrange.layout("", relX(0.20), relY(0), relW(0.65), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_NONE);
  indicator_arrange.draw("    ARRANGE");
  
  decoRight.layout("MUTE", relX(0.85), relY(0), relW(0.15), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, relW(0.02));
  decoRight.draw();
  
  indicator_patternLength.layout("LENGTH", relX(0.05), relY(0.9), relW(0.40), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_patternLength.cb = &patternLengthClick;
  indicator_patternLength.draw(4,0,0, true);
    
  indicator_patternPosition.layout("POSITION", relX(0.55), relY(0.9), relW(0.40), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_patternPosition.draw(0,0,0, true);
}

bool Arrangement::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_patternLength.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}
