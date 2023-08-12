#pragma once

#include "track.h"
#include "pattern.h"
#include "arrangement.h"
#include "ui.h"
#include "peripherals.h"
#include "x_globals.h"

#define SEQ_STOPPED  0
#define SEQ_PLAYING  1
#define SEQ_PRECOUNT 2

extern uint8_t currentProject;
extern uint8_t currentTrack;
extern uint8_t currentPattern;
extern uint8_t currentArrangementPosition;
extern Pattern patterns[NR_PATTERNS];
extern Arrangement arrangement;

struct Transport
{
  uint8_t   bpm         = 120;
  uint16_t  oneTickUs   = 1000 * 60000 / (120 * RESOLUTION);
  uint8_t   state       = SEQ_STOPPED;
  bool      recording   = false;
  bool      metronomeOn = true;
  uint8_t   metronomeChannel = 10;
  uint8_t   metronomeNote1 = 76;
  uint8_t   metronomeNote2 = 77;
  uint8_t   signatureId = 0;
  char      signature[6]  = "4/4";
  uint16_t  ticksPerBar = RESOLUTION * 4;
  uint16_t  ticksPerBeat = RESOLUTION;
  uint16_t  trp_bar     = 0;
  uint16_t  trp_4th     = 0;
  uint16_t  trp_16th    = 0;
  uint16_t  trp_768th   = 0;
  uint16_t  freeMemory  = MEMORY_MAX;
  uint8_t   precountBars = 0;
  uint16_t  precountTicks = 0;
  bool      arrangementOn = false;
  uint32_t  leftLocatorTick = 0;
  uint32_t  rightLocatorTick = 4 * RESOLUTION * 4;
  bool      cycle = false;
  bool      trp_continue = false;
};

extern Transport transport;

void setupSequencer();
void updateSequencer();
void tickPattern();
void play();
void stop();
void seq_continue();
void reset();
void panic();
void record(bool record);
void handlePrecount();

void processInput(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);

void syncTransportSettings();
void setBpm(uint8_t bpm);
void updateTransport(uint32_t tick);
void updateMetronome(bool reset);
void updateFreeMemory();
void clearTrack(uint8_t trackId);
void clearTracks();
void copyTrack();
void pasteTrack();
void clearPatterns();
void clearArrangement();
void setQuantize(uint8_t quantize);
uint8_t getQuantize();
void setMuteStatus();
void setSignature(uint8_t signatureId);
