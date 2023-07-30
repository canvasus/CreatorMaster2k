#include "uiElements.h"

// --- GEO CLASS
Geo::Geo() { }

void Geo::configure(String _label, uint16_t _xPos, uint16_t _yPos, uint16_t _width, uint16_t _height, uint16_t _color1, uint16_t _color2)
{
  label = _label;
  xPos = _xPos;
  yPos = _yPos;
  width = _width;
  height = _height;
  color1 = _color1;
  color2 = _color2;
}

uint16_t Geo::relX(float fraction) { return (uint16_t)(xPos + width * fraction); }
uint16_t Geo::relY(float fraction) { return (uint16_t)(yPos + height * fraction); }
uint16_t Geo::relW(float fraction) { return (uint16_t)(width * fraction); }
uint16_t Geo::relH(float fraction) { return (uint16_t)(height * fraction); }

// --- DECO CLASS ---
Deco::Deco() { }

void Deco::layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t fillColor, uint16_t textColor, int textRelXposition)
{
  _geo.configure(label, xPos, yPos, width, height, fillColor, textColor);
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
  geo.configure(label, xPos, yPos, width, height, bgColor, selectedColor);
}

void Container::draw()
{
  tft.writeTo(L2);
  tft.fillRect(geo.xPos, geo.yPos , geo.width, geo.height, geo.color1);
  if (drawBorder) tft.drawRect(geo.xPos, geo.yPos , geo.width, geo.height, CONTAINER_BORDER_COLOR);
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
  _geo.configure(label, xPos, yPos, width, height, color1, color2);
}

void Button::draw(bool state)
{
  tft.writeTo(L2);
  if (type == TYPE_BUTTON)
  {
    const int8_t mainOffset[4] = {5, 5, -10, -10};
    const int8_t shadowOffset[4] = {10, 10, -12, -12};
    tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BG_COLOR); // background
    if (drawBorder) tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, BUTTON_BORDER_COLOR); // outer border
    
    if (!state) //not pressed
    {
      tft.fillRect(_geo.xPos + shadowOffset[0], _geo.yPos + shadowOffset[1] , _geo.width + shadowOffset[2], _geo.height + shadowOffset[3], BUTTON_SHADOW_COLOR);  //shadow
      tft.fillRect(_geo.xPos + mainOffset[0], _geo.yPos + mainOffset[1] , _geo.width + mainOffset[2], _geo.height + mainOffset[3], _geo.color1); // main button area (state off)
      tft.drawRect(_geo.xPos + mainOffset[0], _geo.yPos + mainOffset[1] , _geo.width + mainOffset[2], _geo.height + mainOffset[3], BUTTON_BORDER_COLOR); // main button border
    }
    
    else // pressed
    {
      tft.fillRect(_geo.xPos + shadowOffset[0], _geo.yPos + shadowOffset[1] , _geo.width + shadowOffset[2], _geo.height + shadowOffset[3], BUTTON_BG_COLOR);  // clear shadow
      tft.fillRect(_geo.xPos + mainOffset[0], _geo.yPos + mainOffset[1] , _geo.width + mainOffset[2], _geo.height + mainOffset[3], _geo.color2); // main button area (state on)
      tft.drawRect(_geo.xPos + mainOffset[0], _geo.yPos + mainOffset[1] , _geo.width + mainOffset[2], _geo.height + mainOffset[3], BUTTON_BORDER_COLOR); // main button border
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
    //state = !state;
    //draw(state);
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
  _geo.configure(label, xPos, yPos, width, height, color1, color2);
  _labelPosition = labelPosition;
}

void Indicator::setLabelOffset(int xOffset, int yOffset)
{
  _labelXoffset = xOffset;
  _labelYoffset = yOffset;
}

void Indicator::setVariableOffset(int xOffset, int yOffset)
{
  _variableXoffset = xOffset;
  _variableYoffset = yOffset;
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
  //tft.setCursor(_geo.xPos + 5, _geo.yPos + 2);
  tft.setCursor(_geo.xPos + _variableXoffset, _geo.yPos + _variableYoffset); // set position for main text
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

void Indicator::draw(uint32_t value)
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
  _geo.configure("", xPos, yPos, width, height, color1, color2);

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
  _geo.configure("", xPos, yPos, width, height, color1, color2);
}

void TrackRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  if (selected) tft.setTextColor(TRACK_SELECTED_TEXT_COLOR);
  else tft.setTextColor(TRACK_TEXT_COLOR);
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
  tft.fillRect(_geo.xPos + 3, _geo.yPos + 2, a_width , _geo.height - 4, ACTIVITY_BG_COLOR);
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
    //String empty = "<empty>";
    String empty = "";
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

  indicator_port.layout("PORT", relX(0.66), relY(1/17.0), relW(0.32), relH(1/17.5) , BUTTON_FILL_NORMAL, BUTTON_FILL_NORMAL, INDICATOR_LABEL_CUSTOM);
  indicator_port.cb = &portClick;
  indicator_port.setLabelOffset(-74, 2);
  indicator_port.draw(portStrings[0]);

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
  if (indicator_port.checkCursor(xPos, yPos, clickType)) return true;
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

void TrackDetailsView::update(uint8_t portIndex, uint8_t trackNr, uint8_t channel, uint8_t quantizeIndex, int transpose, uint8_t loop, int velocity, uint8_t compressIndex, uint8_t lengthIndex)
{
  indicator_trackNr.draw(trackNr + 1);
  indicator_port.draw(portStrings[portIndex]);
  indicator_channel.draw(channel);
  indicator_quantize.draw(quantizeStrings[quantizeIndex]);
  indicator_transpose.draw(transpose);
  indicator_loop.draw(loop);
  indicator_velocity.draw(velocity);
  indicator_compress.draw(compressStrings[compressIndex]);
  indicator_length.draw(lengthStrings[lengthIndex]);
  //indicator_port.draw(portStrings[portIndex]);)
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
  button_stop.draw(true);
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
  _geo.configure("", xPos, yPos, width, height, color1, color2);
}

void ArrangementRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  if (selected) tft.setTextColor(ARRITEM_SELECTED_TEXT_COLOR);
  else tft.setTextColor(ARRITEM_TEXT_COLOR);
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
  indicator_patternPosition.draw(1,0,0, true);

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

// --- FILE MANAGER ---

void FileManagerView::layout()
{
  button_loadPatterns.layout(F("LOAD PTRNS"), relX(0.80), relY(0.6), relW(0.19), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_loadPatterns.draw(false);
  button_loadPatterns.cb = &loadPatternsClick;

  button_load.layout(F("LOAD ALL"), relX(0.80), relY(0.7), relW(0.19), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_load.draw(false);
  button_load.cb = &loadClick;

  button_save.layout(F("SAVE"), relX(0.80), relY(0.8), relW(0.19), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_save.draw(false);
  button_save.cb = &saveClick;

  button_exit.layout(F("EXIT"), relX(0.80), relY(0.9), relW(0.19), relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_exit.draw(false);
  button_exit.cb = &exitEditorClick;

  for (uint8_t row = 0; row < NR_FILE_ROWS; row++)
  {
    fileManagerRows[row].id = row;
    fileManagerRows[row].layout(relX(0.1), relY(0.02 + row * 0.96/(float)NR_FILE_ROWS), relW(0.5), relH(0.96/(float)NR_FILE_ROWS), TRACK_NORMAL_COLOR, TRACK_SELECTED_COLOR);
    fileManagerRows[row].draw(fileManagerRows[row].id == selectedIndex);
  }

  keyboard.layout(relX(0.6), relY(0.1), relW(0.4), relH(0.4), BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
}

bool FileManagerView::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_exit.checkCursor(xPos, yPos, clickType)) return true;
  if (button_load.checkCursor(xPos, yPos, clickType)) return true;
  if (button_save.checkCursor(xPos, yPos, clickType)) return true;
  if (button_loadPatterns.checkCursor(xPos, yPos, clickType)) return true;
  
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
  _geo.configure("", xPos, yPos, width, height, color1, color2);
}

void FileManagerRow::draw(bool selected)
{
  tft.writeTo(L2);
  tft.setFontScale(0);
  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
  if (selected) tft.setTextColor(RA8875_WHITE);
  else tft.setTextColor(INDICATOR_TEXT_COLOR);
  tft.printf("PROJECT %2d", id + 1);
}

bool FileManagerRow::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else return true;
}

void OnscreenKeyboard::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.configure("", xPos, yPos, width, height, color1, color2);
  Serial.printf("Keyboard outline: x %d, y %d, w %d, h %d\n", xPos, yPos, width, height);
  for (uint8_t letterId = 0; letterId < 26; letterId++)
  {
    uint8_t row = letterId / 6;
    uint8_t col = letterId % 6;
    uint16_t xPos = _geo.relX(col * 0.15);
    uint16_t yPos = _geo.relY(row * 0.15);
    letters[letterId].layout(String(letterId), xPos, yPos, _geo.relW(0.12), _geo.relH(0.12), BUTTON_FILL_PRESSED, BUTTON_FILL_PRESSED, INDICATOR_LABEL_NONE);
    letters[letterId].draw(String(letterId+65));
    //Serial.printf("Key: %s, x %d, y %d, w %d, h %d\n", String(letterId), xPos, yPos,  _geo.relW(0.15), _geo.relH(0.15));
  }

  for (uint8_t digitId = 0; digitId < 10; digitId++)
  {
    // draw digit buttons
  }
}

bool OnscreenKeyboard::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  return false;
}

// --- EDITOR ---

void Grid::layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height)
{
  _geo.configure("", xPos, yPos, width, height, 0x9CF3, 0x9CF3);
  nrRows = (lastNoteIndex - firstNoteIndex + 1);
  gridSizePixels_x = _geo.width / nrColumns;
  gridSizePixels_y = _geo.height / nrRows;
  actualHeight = gridSizePixels_y * nrRows;
  actualWidth = gridSizePixels_x * nrColumns;
  lastTick = firstTick + nrColumns * (RESOLUTION >> 2);
}

void Grid::draw()
{
  tft.writeTo(L2);
  
  // draw horizontal lines and keybed hints
  for (uint16_t row = 0; row < nrRows; row++)
  {
    uint8_t note = lastNoteIndex - row;
    uint8_t noteInOctave = note % 12;
    switch (noteInOctave)
    {
      case 1:  // C#
      case 3:  // D#
      case 6:  // F#
      case 8:  // G#
      case 10: // A#
        tft.fillRect(_geo.xPos, _geo.yPos + row * gridSizePixels_y, _geo.width, gridSizePixels_y, EDITOR_BLACKKEY_COLOR);
        break;
    }
    tft.drawFastHLine(_geo.xPos, _geo.yPos + row * gridSizePixels_y, _geo.width, _geo.color1);
  }
  tft.drawFastHLine(_geo.xPos, _geo.yPos + nrRows * gridSizePixels_y, _geo.width, _geo.color1);

  // draw vertical lines
  for (uint16_t column = 0; column <= nrColumns; column++)
  {
    tft.drawFastVLine(_geo.xPos + column * gridSizePixels_x, _geo.yPos, actualHeight, _geo.color1);
  }

  drawNotes();
}

void Grid::clear()
{
   tft.writeTo(L2);
   tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, EDITOR_BG_COLOR);
}

void Grid::syncToTrack()
{
  nrVisibleEvents = 0;
  if ( (track != nullptr) && (track->_nrEvents > 0) )
  {
     for (uint16_t eventIndex = 0; eventIndex < track->_nrEvents; eventIndex++)
    {
      if (nrVisibleEvents >= NR_EDITOR_NOTES) break;
      switch (track->events[eventIndex].type)
      {
        case 0x90: // note on
          if (  (track->events[eventIndex].timestamp >= firstTick) &&
                (track->events[eventIndex].timestamp < lastTick) &&
                (track->events[eventIndex].data1 >= firstNoteIndex) &&
                (track->events[eventIndex].data1 <= lastNoteIndex) )
                    {
                      uint16_t noteOffIndex = track->getMatchingNoteOff(eventIndex);
                      uint32_t timestampNoteOn = track->events[eventIndex].timestamp;
                      uint32_t timestampNoteOff = track->events[noteOffIndex].timestamp;
                      if (!(timestampNoteOff > timestampNoteOn)) timestampNoteOff = timestampNoteOn + 1; // TBD, need better handling
                      if (timestampNoteOff > lastTick) timestampNoteOff = lastTick; // to avoid drawing longer notes outside grid
                      noteElements[nrVisibleEvents].eventIndex_noteOn = eventIndex;
                      noteElements[nrVisibleEvents].eventIndex_noteOff = noteOffIndex;
                      noteElements[nrVisibleEvents].xPos = _timestampToXpos(timestampNoteOn);
                      noteElements[nrVisibleEvents].yPos = _noteToYpos(track->events[eventIndex].data1);
                      noteElements[nrVisibleEvents].width = _timestampToWidth(timestampNoteOff - timestampNoteOn);
                      noteElements[nrVisibleEvents].height = gridSizePixels_y - 2;
                      nrVisibleEvents++;
                    }
          break;
        default:
          //Serial.printf("Ignored event: %d\n", track->events[eventIndex].type);
          break;
      }
    }
  }
}

uint16_t Grid::_timestampToXpos(uint32_t timestamp)
{
  timestamp = timestamp - firstTick;
  float fractionalTime = timestamp / (1.0 * (lastTick - firstTick));
  uint16_t xPos = _geo.xPos + fractionalTime * actualWidth; //_geo.width;
  //Serial.printf("Timestamp: %d --> xPos %d\n", timestamp, xPos);
  return xPos;
}

uint16_t Grid::_noteToYpos(uint8_t note)
{
  uint8_t row = lastNoteIndex - note;
  uint16_t yPos = _geo.yPos + row * gridSizePixels_y;
  //Serial.printf("Note: %d --> yPos %d\n", note, yPos);
  return yPos;
}

uint16_t Grid::_timestampToWidth(uint32_t timestamp)
{
  float fractionalWidth = timestamp / (1.0 * (lastTick - firstTick));
  uint16_t width = fractionalWidth * actualWidth;// _geo.width;
  return width;
}

uint8_t Grid::_yPosToNote(uint16_t yPos)
{
  uint8_t row = (yPos - _geo.yPos) / gridSizePixels_y;
  uint8_t note = lastNoteIndex - row;
  return note;
}

uint32_t Grid::_xPosToQuantizedTimestamp(uint16_t xPos)
{
  uint16_t column = (xPos - _geo.xPos) / gridSizePixels_x;
  uint32_t timestamp = firstTick + column * (RESOLUTION >>2);
  return timestamp;
}

void Grid::drawNotes()
{
  for (uint16_t index = 0; index < nrVisibleEvents; index++) noteElements[index].draw(index == selectedNoteId);
}

void Grid::deleteNote(uint16_t noteOnEventIndex)
{
  //uint16_t noteOnEventIndex = noteElements[index].eventIndex_noteOn;
  track->deleteNote(noteOnEventIndex);
  selectedNoteId = -1;
  clear();
  syncToTrack();
  draw();
}

void Grid::addNote(uint8_t note, uint32_t timestampOn, uint32_t timestampOff)
{
  track->addNoteDirect(timestampOn, timestampOff, note, 100);
  clear();
  syncToTrack();
  draw();
  if ( (transport.state == SEQ_STOPPED) && (track->midi_cb != nullptr))
  {
    track->midi_cb(track->config.channel, 0x90, note, 100);
    delay(200);
    track->midi_cb(track->config.channel, 0x80, note, 0);
  }
}

bool Grid::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || xPos > (_geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false; // outside

  for (uint16_t index = 0; index < nrVisibleEvents; index++)
  {
    if (noteElements[index].checkCursor(xPos, yPos, clickType))
    {
      if (clickType == MOUSE_LEFT) // select
      {
        if (selectedNoteId > -1) noteElements[selectedNoteId].draw(false);
        selectedNoteId = index;
        noteElements[selectedNoteId].draw(true);
        return true;
      }
      if (clickType == MOUSE_RIGHT) // delete
      {
        uint16_t noteOnEventIndex = noteElements[index].eventIndex_noteOn;
        deleteNote(noteOnEventIndex);
        return true;
      }
    }
  }

  // inside grid but no note interaction, so add note at position if clickType == 1
  if (clickType == MOUSE_LEFT)
  {
    uint8_t note = _yPosToNote(yPos);
    uint32_t timestampOn = _xPosToQuantizedTimestamp(xPos);
    uint32_t timestampOff = timestampOn + (RESOLUTION >> 2) - 1; // 1/16th, make this variable later
    addNote(note, timestampOn, timestampOff);
    return true;
  }

  return false;
}

void Grid::moveY(bool isUp)
{
  if (isUp)
  {
    uint8_t topMargin = 127 - lastNoteIndex;
    uint8_t deltaUp = min(topMargin, 8);
    lastNoteIndex = lastNoteIndex + deltaUp;
    firstNoteIndex = firstNoteIndex + deltaUp;
  }
  else
  {
    uint8_t deltaDown = min(firstNoteIndex, 8);
    lastNoteIndex = lastNoteIndex - deltaDown;
    firstNoteIndex = firstNoteIndex - deltaDown;
  }
  clear();
  syncToTrack();
  draw();
}

void Grid::moveX(bool isRight)
{
  uint16_t delta = (nrColumns >> 1) * (RESOLUTION >> 2);
  if (isRight)
  {
    firstTick = firstTick + delta;
    lastTick = lastTick + delta;
  }
  else
  {
    delta = min(firstTick, delta);
    firstTick = firstTick - delta;
    lastTick = lastTick - delta;
  }
  clear();
  syncToTrack();
  draw();
}

void Grid::zoomX(bool isIn) // time
{
  if (isIn && nrColumns >= 8) nrColumns = nrColumns - 4;
  else if (nrColumns < EDITOR_MAX_COLUMNS) nrColumns = nrColumns + 4;
  gridSizePixels_x = _geo.width / nrColumns;
  actualWidth = gridSizePixels_x * nrColumns;
  lastTick = firstTick + nrColumns * (RESOLUTION >> 2);
  clear();
  syncToTrack();
  draw();
}

void Grid::zoomY(bool isIn) // note
{
  if (isIn && (nrRows > EDITOR_MIN_ROWS) ) nrRows--; 
  else if (nrRows < EDITOR_MAX_ROWS) nrRows++;
  gridSizePixels_y = _geo.height / nrRows;
  actualHeight = gridSizePixels_y * nrRows;
  lastNoteIndex = firstNoteIndex + nrRows - 1;
  clear();
  syncToTrack();
  draw();
}

void Grid::animate()
{
  static uint32_t lastTimestamp = 0;
  uint32_t newTimestamp = track->uiTimestamp;
  if (lastTimestamp != newTimestamp)
  {
    drawPosition(newTimestamp);
    lastTimestamp = newTimestamp;
  }
}

void Grid::drawPosition(uint32_t timestamp)
{
  if (timestamp < lastTick)
  {
    static uint16_t lastPositionX = 0;
    uint16_t positionX = _timestampToXpos(timestamp);
    tft.writeTo(L1);
    tft.drawFastVLine(lastPositionX, _geo.yPos, actualHeight, RA8875_MAGENTA);
    tft.drawFastVLine(positionX, _geo.yPos, actualHeight, 0x7BEF);
    lastPositionX = positionX;
  }
}

void Grid::clearPosition()
{
  tft.writeTo(L1);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, RA8875_MAGENTA);
}

void NoteElement::draw(bool selected)
{
  tft.writeTo(L2);
  if (selected) tft.fillRect(xPos, yPos + 1 , width, height, EDITOR_NOTE_COLOR_SELECTED);
  else tft.fillRect(xPos, yPos + 1 , width, height, EDITOR_NOTE_COLOR_DEFAULT);
  //Serial.printf("Draw note - %d, %d, %d, %d\n", xPos, yPos, width, height);
}

bool NoteElement::checkCursor(uint16_t _xPos, uint16_t _yPos, uint8_t clickType)
{
  //const uint8_t edgeWidth = 4;
  if ( (_xPos < xPos) || (_xPos > xPos + width) || (_yPos < yPos) || (_yPos > yPos + height) ) return false; // outside

  // if ( _xPos <= (xPos + edgeWidth) ) // left edge
  // {
  //   Serial.println("Left edge");
  //   return true;
  // }

  // if ( (_xPos > xPos + edgeWidth) && ( _xPos < xPos + width - edgeWidth) ) // center
  // {
  //   Serial.println("Center");
  //   return true;
  // }

  // if (  _xPos >= (xPos + width - edgeWidth) ) // right edge
  // {
  //   Serial.println("Right edge");
  //   return true;
  // } 

  return true;
}

void GraphicEditor::layout()
{
  uint16_t gridHeight = geo.relH(0.8);
  uint16_t gridWidth = geo.relW(0.8);

  button_exit.layout("EXIT", geo.relX(0.91), geo.relY(0.9), geo.relW(0.08), geo.relH(0.1) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_exit.cb = &exitEditorClick;

  // Time / X
  button_gridMoveLeft.layout("<", geo.relX(0.08), geo.relY(0.02), geo.relW(0.07), geo.relH(0.07) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridMoveLeft.setLabelOffset(8, 4);

  button_gridMoveRight.layout(">", geo.relX(0.8), geo.relY(0.02), geo.relW(0.07), geo.relH(0.07) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridMoveRight.setLabelOffset(8, 4);

  button_gridZoomX.layout("Z", geo.relX(0.45), geo.relY(0.02), geo.relW(0.10), geo.relH(0.07) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridZoomX.setLabelOffset(8, 4);

  // Note / Y
  button_gridMoveUp.layout("^", geo.relX(0.01), geo.relY(0.08), geo.relW(0.04), geo.relH(0.08) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridMoveUp.setLabelOffset(8, 8);

  button_gridZoomY.layout("Z", geo.relX(0.01), geo.relY(0.40), geo.relW(0.04), geo.relH(0.14) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridZoomY.setLabelOffset(8, 8);

  button_gridMoveDown.layout("V", geo.relX(0.01), geo.relY(0.8), geo.relW(0.04), geo.relH(0.08) , BUTTON_FILL_NORMAL, BUTTON_FILL_PRESSED);
  button_gridMoveDown.setLabelOffset(8, 8);

  grid.layout(geo.relX(0.08), geo.relY(0.10), gridWidth, gridHeight);

  indicator_lastNote.layout("", geo.relX(0.01), geo.relY(0.02), geo.relW(0.04), geo.relH(0.04), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_NONE);
  indicator_firstNote.layout("", geo.relX(0.01), geo.relY(0.90), geo.relW(0.04), geo.relH(0.04), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_NONE);
  indicator_lastNote.setVariableOffset(2, 1);
  indicator_firstNote.setVariableOffset(2, 1);

  indicator_noteValue.layout("Note", geo.relX(0.89), geo.relY(0.1), geo.relW(0.092), geo.relH(0.05), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_noteOnTick.layout("OnT", geo.relX(0.89), geo.relY(0.2), geo.relW(0.092), geo.relH(0.05), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);
  indicator_noteOffTick.layout("OffT", geo.relX(0.89), geo.relY(0.3), geo.relW(0.092), geo.relH(0.05), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR, INDICATOR_LABEL_TOP);

  indicator_noteValue.cb = &editor_noteValueClick;
  indicator_noteOnTick.cb = &editor_noteOnTickClick;
  indicator_noteOffTick.cb = &editor_noteOffTickClick;
}

void GraphicEditor::draw()
{
  tft.writeTo(L2);
  tft.fillRect(geo.xPos, geo.yPos , geo.width, geo.height, EDITOR_BG_COLOR); // background
  tft.drawRect(geo.xPos, geo.yPos , geo.width, geo.height, EDITOR_BORDER_COLOR); // border

  button_exit.draw(false);
  button_gridMoveLeft.draw(false);
  button_gridMoveRight.draw(false);
  button_gridZoomX.draw(false);
  button_gridZoomY.draw(false);

  button_gridMoveUp.draw(false);
  button_gridMoveDown.draw(false);

  grid.draw();

  if (grid.selectedNoteId > -1) drawNoteInfo();
  else
  {
    indicator_noteValue.draw("-");
    indicator_noteOnTick.draw("-");
    indicator_noteOffTick.draw("-");
  }

  indicator_firstNote.draw(grid.firstNoteIndex);
  indicator_lastNote.draw(grid.lastNoteIndex);
}

void GraphicEditor::drawNoteInfo()
{
  if (grid.selectedNoteId > -1)
  {
    uint16_t eventIndex_noteOn = grid.noteElements[grid.selectedNoteId].eventIndex_noteOn;
    uint16_t eventIndex_noteOff = grid.noteElements[grid.selectedNoteId].eventIndex_noteOff;
    uint32_t noteOnTick = grid.track->events[eventIndex_noteOn].timestamp;
    uint32_t noteOffTick = grid.track->events[eventIndex_noteOff].timestamp;
    uint8_t noteValue = grid.track->events[eventIndex_noteOn].data1;
    
    indicator_noteValue.draw(noteValue);
    indicator_noteOnTick.draw(noteOnTick);
    indicator_noteOffTick.draw(noteOffTick);
  }
  else
  {
    indicator_noteValue.draw("-");
    indicator_noteOnTick.draw("-");
    indicator_noteOffTick.draw("-");
  }
}

void GraphicEditor::drawNoteRange()
{
  indicator_firstNote.draw(grid.firstNoteIndex);
  indicator_lastNote.draw(grid.lastNoteIndex);
}

void GraphicEditor::setTrack(Track * _track)
{
  track = _track;
  grid.track = _track;
}

bool GraphicEditor::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_exit.checkCursor(xPos, yPos, clickType)) return true;

  if (button_gridMoveLeft.checkCursor(xPos, yPos, clickType))
  {
    if (clickType == MOUSE_LEFT)
    {
      grid.moveX(false);
      drawNoteRange();
    }
    return true;
  } 
  
  if (button_gridMoveRight.checkCursor(xPos, yPos, clickType))
  {
    if (clickType == MOUSE_LEFT)
    {
      grid.moveX(true);
      drawNoteRange();
    }
    return true;
  } 
  
  if (button_gridMoveUp.checkCursor(xPos, yPos, clickType))
  { 
    if (clickType == MOUSE_LEFT)
    {
      grid.moveY(true);
      drawNoteRange();
    }
    return true;
  } 
  
  if (button_gridMoveDown.checkCursor(xPos, yPos, clickType))
  {
    if (clickType == MOUSE_LEFT)
    {
      grid.moveY(false);
      drawNoteRange();
    }
    return true;
  } 

  if (button_gridZoomX.checkCursor(xPos, yPos, clickType))
  {
    grid.zoomX(clickType == MOUSE_LEFT || clickType == MOUSE_WHL_UP);
    drawNoteRange();
    return true;
  }

  if (button_gridZoomY.checkCursor(xPos, yPos, clickType))
  {
    grid.zoomY(clickType == MOUSE_LEFT || clickType == MOUSE_WHL_UP);
    drawNoteRange();
    return true;
  }

  if (grid.checkCursor(xPos, yPos, clickType))
  {
    if (grid.selectedNoteId > -1) drawNoteInfo();
    return true;
  }

  if (grid.selectedNoteId > -1)
  {
    if ( (indicator_noteValue.checkCursor(xPos, yPos, clickType)) || (indicator_noteOnTick.checkCursor(xPos, yPos, clickType)) || (indicator_noteOffTick.checkCursor(xPos, yPos, clickType)) )
    {
      grid.clear();
      grid.syncToTrack();
      grid.draw();
      drawNoteInfo();
      return true;
    } 
  }

  return false;
}
