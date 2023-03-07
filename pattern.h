#pragma once

#include "track.h"
#include "peripherals.h"

#define NR_TRACKS 1

class Pattern
{
  private:
    
  public:
    Pattern();
    uint16_t lengthBeats;
    uint16_t patternTick;
    Track tracks[NR_TRACKS];
    void tick();

};
