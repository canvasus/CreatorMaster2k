#include "sequencer.h"

SequencerParameters sequencerParameters;
Transport transport;
IntervalTimer sequencerUpdateTimer;
//uint8_t sequencerState = SEQ_STOPPED;
//bool sequencerRecording = false;
uint32_t currentTick = 0;
uint8_t currentTrack = 0;
uint8_t currentPattern = 0;
bool midiThrough =  true;

Pattern patterns[NR_PATTERNS];
MIDIcallback  metronome_noteOn_cb;
MIDIcallback  metronome_noteOff_cb;
uint8_t metronomeChannel = 10;
uint8_t metronomeNote1 = 68;
uint8_t metronomeNote2 = 56;

void setupSequencer()
{
  setBpm(sequencerParameters.bpm);
  metronome_noteOn_cb = serialMidiNoteOn;
  metronome_noteOff_cb = serialMidiNoteOff;
}

void updateSequencer()
{
  updateTransport(currentTick);
  updateMetronome();
}

void tickPattern()
{
  if (transport.state != SEQ_STOPPED)
  {
    currentTick++;
    if (currentTick >= patterns[currentPattern].lengthBeats * RESOLUTION) currentTick = 0;
    patterns[0].tick();
  }
}

void play() { transport.state = SEQ_PLAYING;}
void stop() { transport.state = SEQ_STOPPED;}
void reset()
{
  currentTick = 0;
  patterns[0].reset();
}

void record(bool record) { transport.recording = record; }

void setBpm(uint8_t bpm)
{
  sequencerParameters.bpm = bpm;
  uint16_t oneTickUs = 1000 * 60000 / (sequencerParameters.bpm * RESOLUTION);
  sequencerUpdateTimer.begin(tickPattern, oneTickUs); 
}

void processNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  uint32_t timestamp = patterns[currentPattern].patternTick;
  if (patterns[currentPattern].tracks[currentTrack].noteOn_cb && midiThrough) patterns[currentPattern].tracks[currentTrack].noteOn_cb(patterns[currentPattern].tracks[currentTrack].channel, note, velocity);
  if (transport.recording) patterns[currentPattern].tracks[currentTrack].addEvent(timestamp, usbMIDI.NoteOn, note, velocity);
}

void processNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  uint32_t timestamp = patterns[currentPattern].patternTick;
  if (patterns[currentPattern].tracks[currentTrack].noteOff_cb && midiThrough) patterns[currentPattern].tracks[currentTrack].noteOff_cb(patterns[currentPattern].tracks[currentTrack].channel, note, velocity);
  if (transport.recording) patterns[currentPattern].tracks[currentTrack].addEvent(timestamp, usbMIDI.NoteOff, note, velocity);
}

void updateTransport(uint32_t tick)
{
  transport.trp_bar = (uint16_t)(tick / (RESOLUTION * 4));
  tick = tick - transport.trp_bar * RESOLUTION * 4;
  transport.trp_4th = (uint16_t)(tick / RESOLUTION);
  tick = tick - transport.trp_4th * RESOLUTION;
  transport.trp_16th = (uint16_t)(tick / (RESOLUTION / 4));
}

void updateMetronome()
{
  static uint16_t last_4th = 0;
  uint8_t note = 64;
  elapsedMillis timer = 0;
  bool played = false;
  
  if (last_4th != transport.trp_4th)
  {
    last_4th = transport.trp_4th;
    if (transport.trp_4th == 0) note = metronomeNote1;
    else note = metronomeNote2;
    metronome_noteOn_cb(metronomeChannel, note, 80);
    timer = 0;
    played = true;
  }
  
  if (timer > 50 && played)
  {
    played = false;
    metronome_noteOff_cb(metronomeChannel, note, 0);
  }
}

void clearTrack(uint8_t trackId)
{
  patterns[currentPattern].tracks[trackId].clear();
}
