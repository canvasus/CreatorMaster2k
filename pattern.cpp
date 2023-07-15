#include "pattern.h"

#include "track.h"
#include "sequencer.h"

Pattern::Pattern()
{
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    tracks[trackId].config.channel = trackId + 1;
    tracks[trackId].setMidiCb(serialMidiSend);
    tracks[trackId].config.portIndex = 0;
  }
}

void Pattern::syncPortSettings(uint8_t trackId)
{
  switch (tracks[trackId].config.portIndex)
  {
    case 0:
      tracks[trackId].setMidiCb(serialMidiSend);
      break;
    case 1:
      tracks[trackId].setMidiCb(usbMidi1Send);
      break;
    case 2:
      tracks[trackId].setMidiCb(usbMidi2Send);
      break;
    case 3:
      tracks[trackId].setMidiCb(usbMidi3Send);
      break;
  }
}

void Pattern::tick()
{
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++) trackActivity[trackId] = trackActivity[trackId] + tracks[trackId].triggerEvents(patternTick);
  patternTick++;
}

void Pattern::reset()
{
  patternTick = 0;
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++) tracks[trackId].reset();
}

uint16_t Pattern::getActivity(uint8_t trackId)
{
  uint16_t activity = trackActivity[trackId];
  trackActivity[trackId] = 0;
  return activity;
}

void Pattern::setActivity(uint8_t trackId, uint8_t activity) { trackActivity[trackId] = activity; }

void Pattern::setMuteStatus(uint8_t trackId, bool muteOn) { tracks[trackId].config.hidden = muteOn; }

void Pattern::clear()
{
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++) tracks[trackId].clear();
}
