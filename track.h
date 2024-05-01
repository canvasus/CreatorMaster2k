#pragma once

#include <Arduino.h>
#include "x_globals.h"

#define NONE 0
#define TYPE_NOTEON_TEMP  usbMIDI.NoteOn - 1
#define TYPE_NOTEOFF_TEMP usbMIDI.NoteOff -1
#define TYPE_CONTROLCHANGE_TEMP usbMIDI.ControlChange -1
#define TYPE_PITCHBEND_TEMP usbMIDI.PitchBend -1

typedef void (*MIDIcallback)(uint8_t channel, uint8_t data1, uint8_t data2);
typedef void (*MIDIcallbackGeneric)(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);

struct event
{
  uint32_t  timestamp;
  uint8_t   type;
  uint8_t   data1;
  uint8_t   data2;
};

struct TrackConfig
{
  uint8_t   portIndex = 0;
  uint8_t   channel = 1;
  uint8_t   quantizeIndex = 0;
  int       transpose = 0;
  uint8_t   loop = 0;
  int       velocity = 0;
  uint8_t   compressIndex = 0;
  uint8_t   lengthIndex = 0;
  bool      hidden = false;
  bool      muted = false;

  bool      generatorOn = false;
  char      name[13];
  //uint8_t ghostOf = 0;
};

class Generator
{
  private:
    uint8_t _matrix[16][16];
    uint8_t _currentStep;
  public:
    Generator();
    uint16_t stepLength;
    uint16_t sequenceLength;
    uint16_t noteLength;
    event getEvent(uint32_t timestamp);
    void reset();
};


class Track
{
  private:
    void      _sortEvents();
    uint16_t  _nrTempEvents;
    uint8_t   _loopCounter;
    void      _convertTempEvents();
    void      _initBuffer();
    void      _releaseBuffer();
    void      _expandBuffer();
    uint32_t  _quantizeTimestamp(uint32_t timestamp);
    uint8_t   _processVelocity(uint8_t velocityIn);
    bool      _noteStatus[128];
    uint8_t   _notesPlaying;
    uint16_t  _loopInUse = 0;
    uint8_t   _quantizeInUse = 0;

  public:
    Track();
    TrackConfig config;
    uint16_t  _nrEvents;
    uint16_t _nrGhostEvents;
    uint16_t  quantize = 1;
    uint16_t  memUsage = 0;
    uint8_t   memBlocks = 0;
    uint8_t   compress = 0;
    uint16_t  length = 0;

    uint32_t  uiTimestamp = 0;
    bool  isUserNamed = false;

    MIDIcallbackGeneric  midi_cb;
    void setMidiCb(MIDIcallbackGeneric cb);

    volatile uint16_t nextEventId;
    event * events;
    event * ghostEvents;

    uint16_t  addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2);
    uint16_t  addNoteDirect(uint32_t timestampOn, uint32_t timestampOff, uint8_t note, uint8_t velocity);
    void      clear();
    void      paste(event * eventClipboard, uint16_t nrEvents);
    event *   copy();
    uint16_t  getNrEvents();
    uint32_t  getEventTimestamp(uint16_t eventIndex);
    void      setPosition(uint32_t timestamp);
    void      reset();
    void      cleanupNoteOff();
    void      triggerEvent(uint8_t type, uint8_t data1, uint8_t data2);
    uint16_t  triggerEvents(uint32_t timestamp);
    void      syncSettings();
    uint16_t  getMatchingNoteOff(uint16_t noteOnEventIndex);
    uint16_t  deleteNote(uint16_t noteOnEventIndex);
    void      setAutoName();
};

int compareEvents(const void *s1, const void *s2);
