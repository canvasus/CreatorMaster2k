#pragma once

#include "track.h"
#include "pattern.h"

#define NR_PATTERNS 1
#define RESOLUTION 192 // ticks per quarter note

#define SEQ_STOPPED 0
#define SEQ_PLAYING 1

extern bool sequencerRecording;
extern uint8_t currentTrack;
extern uint8_t currentPattern;

struct SequencerParameters
{
  uint8_t bpm = 120;
};

void setupSequencer();
void updateSequencer();
void tickPattern();
void play();
void stop();
void record(bool record);
void processNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void processNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void setBpm(uint8_t bpm);
void sendMetronome();
void clearTrack(uint8_t trackId);
