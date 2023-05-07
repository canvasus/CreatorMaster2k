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
  char      name[8];
  uint8_t   channel = 1;
  uint8_t   quantizeIndex = 0;
  int       transpose = 0;
  uint8_t   loop = 0;
  int       velocity = 0;
  uint8_t   compressIndex = 0;
  uint8_t   lengthIndex = 0;
  bool      hidden = false;
  bool      muted = false;
};

class Track
{
  private:
    void      _sortEvents();
    uint16_t  _nrEvents;
    uint8_t   _loopCounter;
    void      _convertTempEvents();
    void      _initBuffer();
    void      _releaseBuffer();
    void      _expandBuffer();
    uint32_t  _quantizeTimestamp(uint32_t timestamp);
    uint8_t   _processVelocity(uint8_t velocityIn);
    bool      _noteStatus[128];
    uint8_t   _notesPlaying;
  public:
    Track();
    TrackConfig config;
    uint16_t  quantize = 1;
    uint16_t  memUsage = 0;
    uint8_t   memBlocks = 0;
    uint8_t   compress = 0;
    uint16_t  length = 0;

    MIDIcallbackGeneric  midi_cb;
    void setMidiCb(MIDIcallbackGeneric cb);

    uint16_t nextEventId;
    event * events;
    uint16_t addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2);
    void clear();
    void paste(event * eventClipboard, uint16_t nrEvents);
    event * copy();
    uint16_t getNrEvents();
    uint32_t getEventTimestamp(uint16_t eventIndex);
    void setPosition(uint32_t timestamp);
    //uint8_t getEventType(uint16_t eventIndex);
    //uint8_t getEventData1(uint16_t eventIndex);
    //uint8_t getEventData2(uint16_t eventIndex);
    void reset();
    void cleanupNoteOff();
    void triggerEvent(uint16_t eventIndex);
    uint16_t triggerEvents(uint32_t timestamp);
    void printEventArray(uint8_t lastIndex);
};

int compareEvents(const void *s1, const void *s2);
