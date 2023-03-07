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
  tft.setCursor(geo.xPos + 10, geo.yPos + 10);
  tft.print(geo.label);
}

bool Container::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < geo.xPos) || (xPos > geo.xPos + geo.width) || (yPos < geo.yPos) || (yPos > geo.yPos + geo.height) ) return false;
  else
  {
    Serial.print("Clicked in container ");
    Serial.println(geo.label);
    return checkChildren(xPos, yPos, clickType);
  }
}

bool Container::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  Serial.println("used Container::checkChildren");
  return false;
}

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
  if (!_state) //not pressed
  {
    tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 10, _geo.height - 10, RA8875_BLACK);  //shadow
    tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color1); // main button area
  }
  else
  {
    tft.fillRect(_geo.xPos + 10, _geo.yPos + 10 , _geo.width - 11, _geo.height - 11, BUTTON_BG_COLOR);  // clear shadow
    tft.fillRect(_geo.xPos + 5, _geo.yPos + 5 , _geo.width - 10, _geo.height - 10, _geo.color2); // main button area
  }

  //tft.setFontScale(1);
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(_geo.xPos + 10, _geo.yPos + 10);
  tft.print(_geo.label);
  Serial.printf("Button at X: %d, Y %d, W: %d, H: %d\n", _geo.xPos, _geo.yPos , _geo.width, _geo.height);
}

bool Button::checkCursor(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if ( (xPos < _geo.xPos) || (xPos > _geo.xPos + _geo.width) || (yPos < _geo.yPos) || (yPos > _geo.yPos + _geo.height) ) return false;
  else
  {
    if (cb) cb(clickType);
    _state = !_state;
    draw(_state);
    return true;
  }
}

// --- CONTROLS ---

void Controls::layout()
{
  button_start.layout("START", (uint16_t)(geo.xPos + 0.02 * geo.width), geo.yPos + (uint16_t)(geo.height * 0.9), (uint16_t)(geo.width * 0.56),(uint16_t)(geo.height * 0.1) , RA8875_BLUE, RA8875_RED);
  button_start.draw(false);
  button_start.cb = &testClick;
  button_continue.layout("CONT", (uint16_t)(geo.xPos + 0.6 * geo.width), geo.yPos + (uint16_t)(geo.height * 0.9), (uint16_t)(geo.width * 0.36),(uint16_t)(geo.height * 0.1) , RA8875_BLUE, RA8875_RED);
  button_continue.draw(false);
  button_continue.cb = &testClick;
}

bool Controls::checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType)
{
  if (button_start.checkCursor(xPos, yPos, clickType) ||
      button_continue.checkCursor(xPos, yPos, clickType) )
      return true;
  else return false;
}
