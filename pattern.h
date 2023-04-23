#pragma once

#include "track.h"
#include "peripherals.h"
#include "x_globals.h"

// SAVE:
// name, all tracks

class Pattern
{
  public:
    Pattern();
    char name[8];
    uint16_t patternTick;
    Track tracks[NR_TRACKS];
    void tick();
    void reset();
    uint8_t trackActivity[NR_TRACKS];
    uint16_t getActivity(uint8_t trackId);
    void setMuteStatus(uint8_t trackId, bool muteOn);
    void clear();
};
