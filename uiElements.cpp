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
  tft.setFontScale(0);
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
  if (type == TYPE_BUTTON)
  {
    tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BG_COLOR); // background
    if (drawBorder) tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BORDER_COLOR); // border
    if (!state) //not pressed
    {
      tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 12, _geo.height - 12, BUTTON_SHADOW_COLOR);  //shadow
      tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color1); // main button area (state off)
      tft.drawRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, BUTTON_BORDER_COLOR); // main button border
    }
    else // pressed
    {
      tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 12, _geo.height - 12, BUTTON_BG_COLOR);  // clear shadow
      tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color2); // main button area (state on)
      tft.drawRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, BUTTON_BORDER_COLOR); // main button border
    }
  }
  if (type == TYPE_CHECKBOX)
  {
    uint8_t checkbox_side =  _geo.height - 10;
    uint16_t checkbox_x =  _geo.xPos + _geo.width - 2 * checkbox_side;
    uint16_t checkbox_y =  _geo.yPos + 5;
    tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BG_COLOR); // background
    if (drawBorder) tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BORDER_COLOR); // border
    tft.drawRect(checkbox_x, checkbox_y , checkbox_side, checkbox_side, BUTTON_BORDER_COLOR); // checkbox border
    if (state) tft.fillRect(checkbox_x + 2, checkbox_y + 2 , checkbox_side - 4, checkbox_side - 4, BUTTON_BORDER_COLOR); // checkbox ON
    else tft.fillRect(checkbox_x + 2, checkbox_y + 2 , checkbox_side - 4, checkbox_side - 4, BUTTON_BG_COLOR); // checkbox OFF
  }
  tft.setFontScale(0);
  tft.setTextColor(BUTTON_TEXT_COLOR);
  tft.setCursor(_geo.xPos + _labelXoffset, _geo.yPos + _labelYoffset);
  tft.print(_geo.label);
}

bool Button::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    state = !state;
    draw(state);
    if (cb) cb(clickType);
    return true;
  }
}

void Button::set(bool newState)
{
  state = newState;
  draw(state);
}

void Button::setLabelOffset(int xOffset, int yOffset)
{
  _labelXoffset = xOffset;
  _labelYoffset = yOffset;
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
  tft.setFontScale(0);
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
  tft.writeTo(L2);
  if (drawStatics) _drawCommon();
  else tft.fillRect(_geo.xPos + 1, _geo.yPos + 1 , _geo.width - 2, _geo.height - 2, INDICATOR_BG_COLOR);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.setFontScale(0);
  tft.setCursor(_geo.xPos + _geo.width * 0.1, _geo.yPos + 2);
  tft.print(trp_bar);
  tft.setCursor(_geo.xPos + _geo.width * 0.5, _geo.yPos + 2);
  tft.print(trp_4th);
  tft.setCursor(_geo.xPos + _geo.width * 0.8, _geo.yPos + 2);
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

Scrollbar::Scrollbar() { }

void Scrollbar::configure(uint8_t orientation, uint16_t maxValue, uint16_t minValue, uint16_t inView)
{
  _orientation = orientation;
  rangeInView = inView;
  max = maxValue;
  min = minValue;
  position = min + rangeInView;
}

void Scrollbar::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;

  indicator_up.layout("", _geo.xPos, _geo.yPos, _geo.width, _geo.height /24 , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_NONE);
  indicator_up.draw("A");
  indicator_up.cb = &scrollbarUpClick;
  
  indicator_down.layout("", _geo.xPos, _geo.yPos + _geo.height * 23 / 24, _geo.width, _geo.height /24 , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_NONE);
  indicator_down.draw("V");
  indicator_down.cb = &scrollbarDownClick;
}

void Scrollbar::draw()
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BG_COLOR); // background
  tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BORDER_COLOR); // border
  indicator_up.draw("A");
  indicator_down.draw("V");
}

bool Scrollbar::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  return false;
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
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  if (selected) tft.setTextColor(RA8875_WHITE);
  else tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("%2d", id + 1);
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
  if (level > 0) tft.fillRect(_geo.xPos + 3, _geo.yPos + 4, (uint16_t)( (level/20.0) * a_width) , _geo.height - 8, ACTIVITY_FILL_COLOR);
}

// --- PATTERN VIEW CLASS ---

void PatternView::layout()
{
  indicator_patternName.layout("PATTERN", relX(0.2), relY(0), relW(0.6), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_NONE);
  indicator_patternName.draw("PATRN01");
  indicator_patternName.cb = &patternSelectClick;
  
  decoLeft.layout("STAT", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 4);
  decoLeft.draw();
  
  decoRight.layout("CHN", relX(0.80), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 10);
  decoRight.draw();

  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    String empty = "<empty>";
    trackRows[trackId].cb = &trackSelectClick;
    empty.toCharArray(trackRows[trackId].trackName, 8);
    trackRows[trackId].id = trackId;
    trackRows[trackId].layout(relX(0), relY((1/17.0) * (trackId + 1)), relW(1.0), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
    trackRows[trackId].draw(trackId == currentTrack);
  }
}

bool PatternView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_patternName.checkCursor(xPos, yPos, clickType)) return true;
  
  bool matchClick = false;
  uint8_t trackId = 0;
  while (!matchClick && trackId < NR_TRACKS)
  {
    trackRows[trackId++].checkCursor(xPos, yPos, clickType); 
  }
  return false;
}

// --- TRACK DETAILS CLASS ---

void TrackDetailsView::layout()
{
  decoLeft.layout("", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 0);
  decoLeft.draw();
  
  decoRight.layout("", relX(0.80), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, 0);
  decoRight.draw();
  
  indicator_trackNr.layout("TRACK:", relX(0.6), relY(0), relW(0.2), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_trackNr.setLabelOffset(-48, 2);
  indicator_trackNr.draw(1);

  indicator_channel.layout("CHANNEL", relX(0.66), relY(2/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_channel.cb = &channelClick;
  indicator_channel.setLabelOffset(-74, 2);
  indicator_channel.draw(1);
  
  indicator_quantize.layout("QUANTIZE", relX(0.66), relY(3/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_quantize.cb = &quantizeClick;
  indicator_quantize.setLabelOffset(-74, 2);
  indicator_quantize.draw(quantizeStrings[0]);

  indicator_transpose.layout("TRANSPOSE", relX(0.66), relY(4/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_transpose.cb = &transposeClick;
  indicator_transpose.setLabelOffset(-74, 2);
  indicator_transpose.draw(0);

  indicator_loop.layout("LOOP", relX(0.66), relY(5/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_loop.cb = &loopClick;
  indicator_loop.setLabelOffset(-74, 2);
  indicator_loop.draw("0");

  indicator_velocity.layout("VELOCITY", relX(0.66), relY(6/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_velocity.cb = &velocityClick;
  indicator_velocity.setLabelOffset(-74, 2);
  indicator_velocity.draw("0");

  indicator_compress.layout("COMPRESS", relX(0.66), relY(7/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_compress.cb = &compressClick;
  indicator_compress.setLabelOffset(-74, 2);
  indicator_compress.draw("OFF");

  indicator_length.layout("LENGTH", relX(0.66), relY(8/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_length.cb = &lengthClick;
  indicator_length.setLabelOffset(-74, 2);
  indicator_length.draw("OFF");

  button_copy.layout("COPY", relX(0.03), relY(0.6), relW(0.55), relH(0.09) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL);
  button_copy.cb = &copyTrackClick;
  button_copy.draw(false);

  button_paste.layout("PASTE", relX(0.03), relY(0.7), relW(0.55), relH(0.09) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL);
  button_paste.cb = &pasteTrackClick;
  button_paste.draw(false);

  button_clear.layout("CLEAR", relX(0.03), relY(0.8), relW(0.55), relH(0.09) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL);
  button_clear.cb = &clearTrackClick;
  button_clear.draw(false);

  button_edit.layout("EDIT", relX(0.03), relY(0.9), relW(0.55), relH(0.09) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL);
  button_edit.cb = &editTrackClick;
  button_edit.draw(false);
}

bool TrackDetailsView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_channel.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_quantize.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_transpose.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_loop.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_velocity.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_compress.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_length.checkCursor(xPos, yPos, clickType)) return true;
  if (button_copy.checkCursor(xPos, yPos, clickType)) return true;
  if (button_paste.checkCursor(xPos, yPos, clickType)) return true;
  if (button_edit.checkCursor(xPos, yPos, clickType)) return true;
  if (button_clear.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}

void TrackDetailsView::update(uint8_t trackNr, uint8_t channel, uint8_t quantizeIndex, int transpose, uint8_t loop, int velocity, uint8_t compressIndex, uint8_t lengthIndex)
{
  indicator_trackNr.draw(trackNr + 1);
  indicator_channel.draw(channel);
  indicator_quantize.draw(quantizeStrings[quantizeIndex]);
  indicator_transpose.draw(transpose);
  indicator_loop.draw(loop);
  indicator_velocity.draw(velocity);
  indicator_compress.draw(compressStrings[compressIndex]);
  indicator_length.draw(lengthStrings[lengthIndex]);
}

// --- CONTROLS ---

void ControlsView::layout()
{
  indicator_leftLocator.layout("LEFT LOCATOR", relX(0.25), relY(0.05), TRANSPORT_W, relH(0.06), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_leftLocator.draw(1,0,0, true);
  indicator_leftLocator.cb = &leftLocatorClick;

  indicator_rightLocator.layout("RIGHT LOCATOR", relX(0.25), relY(0.15), TRANSPORT_W, relH(0.06), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_rightLocator.draw(5,0,0, true);
  indicator_rightLocator.cb = &rightLocatorClick;

  indicator_cycle.layout("CYCLE", relX(0.7), relY(0.25), relW(0.2), relH(0.06), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR,INDICATOR_LABEL_LEFT40);
  indicator_cycle.cb = &cycleOnClick;
  indicator_cycle.draw("OFF");
   
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

bool ControlsView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
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

  if (indicator_leftLocator.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_rightLocator.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_cycle.checkCursor(xPos, yPos, clickType)) return true;
  
  return false;
}

// --- HEADER CLASS ---

void HeaderView::layout()
{
  const float button_w = 0.07;
  const float button_h = 0.8;
  const float button_y = 0.18;
  const float button_xPpad = 0.01;
    
  button_file.layout("FILE", relX(button_xPpad), relY(button_y), relW(button_w), relH(button_h) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_file.draw(false);
  button_file.cb = &fileClick;

  button_new.layout("NEW", relX(2 * button_w + 3 * button_xPpad), relY(button_y), relW(button_w), relH(button_h) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_new.draw(false);
  button_new.cb = &newClick;
  
  indicator_freeMem.layout("MEM", relX(0.3), relY(0.5), relW(0.08), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR,INDICATOR_LABEL_TOP);
  indicator_freeMem.draw(MEMORY_MAX);

  indicator_arrOn.layout("ARRANGE", relX(0.4), relY(0.5), relW(0.05), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR,INDICATOR_LABEL_TOP);
  indicator_arrOn.cb = &arrangementOnClick;
  indicator_arrOn.draw("OFF");

  indicator_bpm.layout("BPM", relX(0.5), relY(0.5), relW(0.06), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_bpm.cb = &bpmClick;
  indicator_bpm.draw(transport.bpm);

  indicator_signature.layout("SIGN", relX(0.6), relY(0.5), relW(0.06), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_signature.cb = &signatureClick;
  indicator_signature.draw(transport.signature);

  indicator_transport.layout("bar    4  16", relX(0.85), relY(0.5), TRANSPORT_W, relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP); // relW(0.10)
  indicator_transport.draw(0,0,0, true);
 
}

bool HeaderView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_new.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_bpm.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_arrOn.checkCursor(xPos, yPos, clickType)) return true;
  if (indicator_signature.checkCursor(xPos, yPos, clickType)) return true;
  if (button_file.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}

// --- ARRANGEMENT ROW CLASS ---

ArrangementRow::ArrangementRow()
{
  active = false;
}

void ArrangementRow::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void ArrangementRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  if (selected) tft.setTextColor(RA8875_WHITE);
  else tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("%d", startBars);
  tft.setCursor(_geo.xPos + 50, _geo.yPos + 5);
  tft.print(patternName);
}

void ArrangementRow::clear()
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
}


bool ArrangementRow::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    if (cb) cb(id);
    return true;
  }
}

// --- ARRANGEMENT VIEW CLASS ---

void ArrangementView::layout()
{
  decoLeft.layout("BAR", relX(0.00), relY(0), relW(0.2), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, relW(0.05));
  decoLeft.draw();
  
  indicator_arrange.layout("", relX(0.20), relY(0), relW(0.65), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_NONE);
  indicator_arrange.draw("    ARRANGE");
  
  decoRight.layout("MUTE", relX(0.85), relY(0), relW(0.15), relH(1/17.0), DECO_TOPROW_FILL_COLOR, DECO_TOPROW_TEXT_COLOR, relW(0.02));
  decoRight.draw();
  
  indicator_patternLength.layout("LENGTH", relX(0.05), relY(0.93), relW(0.35), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_patternLength.cb = &patternLengthClick;
  indicator_patternLength.draw(4,0,0, true);
    
  indicator_patternPosition.layout("POSITION", relX(0.48), relY(0.93), relW(0.35), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_patternPosition.draw(0,0,0, true);

  for (uint8_t arrItemId = 0; arrItemId < NR_ARRITEMS; arrItemId++)
  {
    arrangementRows[arrItemId].cb = &arrangementItemSelectClick;
    arrangementRows[arrItemId].patternName = "PATTERN01";
    arrangementRows[arrItemId].id = arrItemId;
    arrangementRows[arrItemId].startBars = 1;
    arrangementRows[arrItemId].layout(relX(0), relY((arrItemId + 1) / 17.0), relW(0.85), relH(1/17.0) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  }
  arrangementRows[0].active = true;
  arrangementRows[0].draw(true);

  button_new.layout("NEW", relX(0.02), relY(0.78), relW(0.2), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_new.draw(false);
  button_new.cb = &newArrangeItemClick;

  button_delete.layout("DEL", relX(0.23), relY(0.78), relW(0.2), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_delete.draw(false);
  button_delete.cb = &deleteArrangeItemClick;

  button_up.layout("UP", relX(0.44), relY(0.78), relW(0.2), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_up.draw(false);
  button_up.cb = &testClick;

  button_down.layout("DN", relX(0.65), relY(0.78), relW(0.2), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_down.draw(false);
  button_down.cb = &testClick;

  for (uint8_t trackIndex = 0; trackIndex < NR_TRACKS; trackIndex++)
  {
    indicator_muteArray[trackIndex].layout("", relX(0.92), relY((trackIndex + 1)/17.0), relW(0.08), relH(1/17.0), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
    indicator_muteArray[trackIndex].draw("-");
    //indicator_muteArray[trackIndex].cb = &muteArrayClick;
  }
}

bool ArrangementView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_patternLength.checkCursor(xPos, yPos, clickType)) return true;
  if (button_new.checkCursor(xPos, yPos, clickType)) return true;
  if (button_delete.checkCursor(xPos, yPos, clickType)) return true;
  for (uint8_t itemIndex = 0; itemIndex < NR_ARRITEMS; itemIndex++) if (arrangementRows[itemIndex].active && arrangementRows[itemIndex].checkCursor(xPos, yPos, clickType)) return true;
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    if (indicator_muteArray[trackId].checkCursor(xPos, yPos, clickType))
    {
      muteArrayClick(trackId);
      return true;
    }
  }
  return false;
}

ListEditorRow::ListEditorRow()
{

}

void ListEditorRow::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void ListEditorRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("%2d", id);
//  tft.setCursor(_geo.xPos + 50, _geo.yPos + 5);
//  tft.print(trackName);
//  tft.setCursor(_geo.xPos + _geo.width - 20, _geo.yPos + 5);
//  tft.print(channel); 
}

void ListEditor::layout()
{
  button_exit.layout("EXIT", relX(0.9), relY(0.9), relW(0.1), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_exit.draw(false);
  button_exit.cb = &exitEditorClick;

  for (uint8_t listItemRow = 0; listItemRow < NR_LIST_ROWS; listItemRow++)
  {
    //listEditorRows[listItemRow].cb = &arrangementItemSelectClick;
    //arrangementRows[arrItemId].patternName = "PATTERN01";
    listEditorRows[listItemRow].id = listItemRow;
    listEditorRows[listItemRow].layout(relX(0), relY((listItemRow + 1) / NR_LIST_ROWS), relW(0.85), relH(1/NR_LIST_ROWS) , TRACK_NORMAL_COLOR, TRACK_SELECTED_COLOR);
  }
  listEditorRows[0].draw(true);
}

bool ListEditor::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_exit.checkCursor(xPos, yPos, clickType)) return true;
  return false;
}



void FileManagerView::layout()
{
  button_exit.layout("EXIT", relX(0.9), relY(0.9), relW(0.1), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_exit.draw(false);
  button_exit.cb = &exitEditorClick;

  button_load.layout("LOAD", relX(0.9), relY(0.7), relW(0.1), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_load.draw(false);
  button_load.cb = &loadClick;

  button_save.layout("SAVE", relX(0.9), relY(0.8), relW(0.1), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_save.draw(false);
  button_save.cb = &saveClick;

  for (uint8_t row = 0; row < NR_FILE_ROWS; row++)
  {
    fileManagerRows[row].id = row;
    fileManagerRows[row].layout(relX(0.1), relY(0.02 + row * 0.96/(float)NR_FILE_ROWS), relW(0.5), relH(0.96/(float)NR_FILE_ROWS), TRACK_NORMAL_COLOR, TRACK_SELECTED_COLOR);
    fileManagerRows[row].draw(fileManagerRows[row].id == selectedIndex);
  }
}

bool FileManagerView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_exit.checkCursor(xPos, yPos, clickType)) return true;
  if (button_load.checkCursor(xPos, yPos, clickType)) return true;
  if (button_save.checkCursor(xPos, yPos, clickType)) return true;
  for (uint8_t row = 0; row < NR_FILE_ROWS; row++)
  {
    if (fileManagerRows[row].checkCursor(xPos, yPos, clickType))
    {
      fileManagerRowClick(row);
      return true;
    }
  }
  return false;
}

FileManagerRow::FileManagerRow()
{
  isActive = false;
}

void FileManagerRow::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void FileManagerRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("PROJECT %2d", id + 1);
}

bool FileManagerRow::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else return true;
}
