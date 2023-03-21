#pragma once

#include "uiElements.h"
#include "x_globals.h"

class ListEditorRow
{
  private:
    Geo     _geo;
  public:
    ListEditorRow();
    uint8_t id;
    Indicator indicator_start_bar;
    Indicator indicator_start_4th;
    Indicator indicator_start_16th;
    Indicator indicator_start_768th; // note: this is one tick with RESOLUTION = 192
    Indicator indicator_type;
    Indicator indicator_channel;
    Indicator indicator_data1;
    Indicator indicator_data2;
    void layout(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint16_t color1, uint16_t color2);
    void draw(bool selected);
};

class ListEditor : public Container
{
  private:
    Geo     _geo;  
  public:
    ListEditor();
    uint16_t firstRowIndex;
    ListEditorRow listEditorRows[NR_LIST_ROWS];
    Button button_exit;
    bool checkChildren(uint16_t xPos, uint16_t yPos, uint8_t clickType);
    void layout();
};
