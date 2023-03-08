#include "uiElements.h"

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
  tft.setTextColor(RA8875_BLACK);
  tft.setCursor(geo.xPos + 5, geo.yPos + 1);
  tft.print(geo.label);
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

Indicator::Indicator() { }

void Indicator::layout(String label, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2)
{
  _geo.label = label;
  _geo.xPos = xPos;
  _geo.yPos = yPos;
  _geo.width = width;
  _geo.height = height;
  _geo.color1 = color1;
  _geo.color2 = color2;
}

void Indicator::_drawCommon()
{
  tft.writeTo(L2);
  tft.fillRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BG_COLOR); // background
  tft.drawRect(_geo.xPos, _geo.yPos , _geo.width, _geo.height, INDICATOR_BORDER_COLOR); // border
  tft.setCursor(_geo.xPos + _geo.width * 0.7, _geo.yPos - 14);
  tft.setTextColor(INDICATOR_LABEL_COLOR);
  tft.print(_geo.label);
  tft.setCursor(_geo.xPos + 2, _geo.yPos + 2);
  tft.setTextColor(INDICATOR_TEXT_COLOR);
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

void Indicator::draw(uint16_t trp_bar, uint16_t trp_4th, uint16_t trp_16th, uint16_t trp_768th)
{
  _drawCommon();
  tft.setCursor(_geo.xPos + _geo.width * 0.05, _geo.yPos + 2);
  tft.print(trp_bar);
  tft.setCursor(_geo.xPos + _geo.width * 0.25, _geo.yPos + 2);
  tft.print(trp_4th);
  tft.setCursor(_geo.xPos + _geo.width * 0.50, _geo.yPos + 2);
  tft.print(trp_16th);
  tft.setCursor(_geo.xPos + _geo.width * 0.75, _geo.yPos + 2);
  tft.print(trp_768th);
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
  if (button_start.checkCursor(xPos, yPos, clickType) ||
      button_continue.checkCursor(xPos, yPos, clickType) ||
      button_stop.checkCursor(xPos, yPos, clickType) ||
      button_reverse.checkCursor(xPos, yPos, clickType) ||
      button_forward.checkCursor(xPos, yPos, clickType) ||
      button_fastReverse.checkCursor(xPos, yPos, clickType) ||
      button_fastForward.checkCursor(xPos, yPos, clickType) ||
      button_punch.checkCursor(xPos, yPos, clickType) ||
      button_record.checkCursor(xPos, yPos, clickType)
      )
      return true;
  else return false;
}

void Header::layout()
{
  indicator_freeMem.layout("free", relX(0.01), relY(0.5), relW(0.1), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR);
  indicator_freeMem.cb = &testClickIndicator;
  indicator_freeMem.draw(12345);

  indicator_transport.layout("free", relX(0.80), relY(0.5), relW(0.15), relH(0.5), INDICATOR_BG_COLOR, INDICATOR_BORDER_COLOR);
  indicator_transport.cb = &testClickIndicator;
  indicator_transport.draw(0,0,0,0);
}

bool Header::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (indicator_freeMem.checkCursor(xPos, yPos, clickType)) return true;
  else return false;
}
