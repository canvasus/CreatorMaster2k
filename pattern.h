#pragma once

#include "track.h"
#include "peripherals.h"
#include "x_globals.h"

class Pattern
{
  private:
    
  public:
    Pattern();
    uint16_t lengthBeats;
    uint16_t patternTick;
    Track tracks[NR_TRACKS];
    void tick();
    void reset();

    uint16_t lastTriggeredEventId[NR_TRACKS];
};
