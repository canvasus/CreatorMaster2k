#pragma once

#include "track.h"
#include "peripherals.h"
#include "x_globals.h"

class Pattern
{
  public:
    Pattern();
    String name;
    uint16_t lengthBeats;
    uint16_t patternTick;
    Track tracks[NR_TRACKS];
    void tick();
    void reset();
    uint8_t trackActivity[NR_TRACKS];
    uint16_t getActivity(uint8_t trackId);
};
