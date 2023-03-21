#include "uiEditor.h"

ListEditorRow::ListEditorRow() { }

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
//  tft.writeTo(L2);
//  if (selected) tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_SELECTED_COLOR); // background
//  else tft.fillRect(_geo.xPos + 1, _geo.yPos , _geo.width - 2, _geo.height, TRACK_NORMAL_COLOR); // background
//  tft.setCursor(_geo.xPos + 20, _geo.yPos + 5);
//  tft.setTextColor(INDICATOR_TEXT_COLOR);
//  tft.printf("%2d", id);
//  tft.setCursor(_geo.xPos + 50, _geo.yPos + 5);
//  tft.print(trackName);
//  tft.setCursor(_geo.xPos + _geo.width - 20, _geo.yPos + 5);
//  tft.print(channel); 
}
