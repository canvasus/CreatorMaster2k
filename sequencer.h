#pragma once

#include "track.h"
#include "pattern.h"
#include "ui.h"

#define NR_PATTERNS 1
#define RESOLUTION 192 // ticks per quarter note

#define SEQ_STOPPED 0
#define SEQ_PLAYING 1

//extern bool sequencerRecording;
extern uint8_t currentTrack;
extern uint8_t currentPattern;

struct Transport
{
  uint8_t state = SEQ_STOPPED;
  bool recording = false;
  uint16_t trp_bar = 0;
  uint16_t trp_4th = 0;
  uint16_t trp_16th = 0;
  uint16_t trp_768th = 0;
};

extern Transport transport;

struct SequencerParameters
{
  uint8_t bpm = 120;
};

void setupSequencer();
void updateSequencer();
void tickPattern();
void play();
void stop();
void reset();
void record(bool record);
void processNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void processNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void setBpm(uint8_t bpm);
void updateTransport(uint32_t tick);
void updateMetronome();
void clearTrack(uint8_t trackId);
