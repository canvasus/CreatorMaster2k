#pragma once

#include <Arduino.h>
#include "x_globals.h"

#define ARRITEM_INACTIVE  0
#define ARRITEM_ACTIVE    1

struct ArrangementItem
{
    uint32_t startTick = 0;
    uint32_t lengthTicks = 4 * 4 * RESOLUTION;
    uint8_t patternIndex = 0;
    uint8_t status = ARRITEM_INACTIVE;
    bool muteArray[NR_TRACKS];
};

class Arrangement
{
  private:
    void _sortItems();
  public:
    Arrangement();
    ArrangementItem arrangementItems_a[NR_ARRITEMS];
    uint8_t newArrangementItem();
    uint8_t deleteArrangementItem(uint8_t index);
    void clear();
    uint32_t arrangementTick;
    uint8_t tick();
    uint8_t getCurrentArrItem();
    void setArrangementPosition(uint8_t positionId, uint16_t startBars, uint8_t patternIndex);
    void updateArrangementStartPositions();
    void printItemArray(uint8_t lastIndex);

    // For later:
    //ArrangementItem arrangementItems_b[NR_ARRITEMS];
    //ArrangementItem arrangementItems_c[NR_ARRITEMS];
    //ArrangementItem arrangementItems_d[NR_ARRITEMS];
};

int compareArrItems(const void *s1, const void *s2);
