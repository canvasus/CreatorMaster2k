#pragma once

#include <Arduino.h>
#include "x_globals.h"

#define NONE 0
#define TYPE_NOTEON_TEMP  usbMIDI.NoteOn - 1
#define TYPE_NOTEOFF_TEMP usbMIDI.NoteOff -1

typedef void (*MIDIcallback)(uint8_t channel, uint8_t note, uint8_t velocity);

struct event
{
  uint32_t  timestamp;
  uint8_t   type;
  uint8_t   data1;
  uint8_t   data2;
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
    uint32_t  _quantizeTimestamp(uint32_t timestamp);
  public:
    Track();
    String    name;
    uint8_t   channel;
    uint16_t  quantize = 1;
    uint8_t   quantizeIndex = 0;
    int       transpose = 0;
    uint8_t   loop = 0;
    uint16_t  memUsage = 0;
    bool      hidden = false;

    MIDIcallback  noteOn_cb;
    MIDIcallback  noteOff_cb;
    void setHandleNoteOn(MIDIcallback cb);
    void setHandleNoteOff(MIDIcallback cb);

    uint16_t nextEventId;
    event * events;
    uint16_t addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2);
    void clear();
    uint32_t getEventTimestamp(uint16_t eventIndex);
    void reset();
    void triggerEvent(uint16_t eventIndex);
    uint16_t triggerEvents(uint32_t timestamp);
    void printEventArray(uint8_t lastIndex);
};

int compareEvents(const void *s1, const void *s2);
