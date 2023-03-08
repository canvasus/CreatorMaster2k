#include "pattern.h"

#include "track.h"
#include "sequencer.h"

Pattern::Pattern()
{
  tracks[0].setHandleNoteOn(serialMidiNoteOn);
  tracks[0].setHandleNoteOff(serialMidiNoteOff);
  lengthBeats = 16;
}

void Pattern::tick()
{
  patternTick++;
  if (patternTick > lengthBeats * RESOLUTION) reset();
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++) tracks[trackId].triggerEvents(patternTick);
}

void Pattern::reset()
{
  patternTick = 0;
  memset(lastTriggeredEventId, 0, NR_TRACKS * sizeof(lastTriggeredEventId[0]));
  tracks[0].reset();
}
