#include "sequencer.h"

SequencerParameters sequencerParameters;
IntervalTimer sequencerUpdateTimer;
uint8_t sequencerState = SEQ_STOPPED;
bool sequencerRecording = false;
uint16_t currentTick = 0;
uint8_t currentTrack = 0;
uint8_t currentPattern = 0;
bool midiThrough =  true;

Pattern patterns[NR_PATTERNS];
MIDIcallback  metronome_noteOn_cb;
uint8_t metronomeChannel = 10;
uint8_t metronomeNote = 4;

void setupSequencer()
{
  setBpm(sequencerParameters.bpm);
  metronome_noteOn_cb = serialMidiNoteOn;
}

void updateSequencer()
{
  static uint8_t beatCounter = 0;
  if (currentTick / RESOLUTION != beatCounter)
  {
    beatCounter = currentTick / RESOLUTION;
    sendMetronome();
    Serial.println(beatCounter + 1);
  }
}

void tickPattern()
{
  if (sequencerState != SEQ_STOPPED)
  {
    currentTick++;
    if (currentTick >= patterns[currentPattern].lengthBeats * RESOLUTION) currentTick = 0;
    patterns[0].tick();
  }
}

void play() { sequencerState = SEQ_PLAYING;}
void stop() { sequencerState = SEQ_STOPPED;}
void record(bool record) { sequencerRecording = record; }

void setBpm(uint8_t bpm)
{
  sequencerParameters.bpm = bpm;
  uint16_t oneTickUs = 1000 * 60000 / (sequencerParameters.bpm * RESOLUTION);
  sequencerUpdateTimer.begin(tickPattern, oneTickUs); 
}

void processNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  uint16_t timestamp = patterns[currentPattern].patternTick;
  if (patterns[currentPattern].tracks[currentTrack].noteOn_cb && midiThrough) patterns[currentPattern].tracks[currentTrack].noteOn_cb(patterns[currentPattern].tracks[currentTrack].channel, note, velocity);
  if (sequencerRecording) patterns[currentPattern].tracks[currentTrack].addEvent(timestamp, usbMIDI.NoteOn, note, velocity);
}

void processNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  uint16_t timestamp = patterns[currentPattern].patternTick;
  if (patterns[currentPattern].tracks[currentTrack].noteOff_cb && midiThrough) patterns[currentPattern].tracks[currentTrack].noteOff_cb(patterns[currentPattern].tracks[currentTrack].channel, note, velocity);
  if (sequencerRecording) patterns[currentPattern].tracks[currentTrack].addEvent(timestamp, usbMIDI.NoteOff, note, velocity);
}

void sendMetronome()
{
  metronome_noteOn_cb(metronomeChannel, metronomeNote, 100);
}

void clearTrack(uint8_t trackId)
{
  patterns[currentPattern].tracks[trackId].clear();
}
