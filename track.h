#pragma once

#include <Arduino.h>

#define NR_EVENTS 128
#define NONE 0

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
    void  _sortEvents();
    uint16_t _nrEvents;
    uint8_t _quantizeCounter;
  public:
    Track();
    uint8_t   channel;
    uint16_t  quantize;
    uint8_t   groove;
    uint8_t   transpose;
    uint8_t   loop;

    MIDIcallback  noteOn_cb;
    MIDIcallback  noteOff_cb;
    void setHandleNoteOn(MIDIcallback cb);
    void setHandleNoteOff(MIDIcallback cb);

    //uint16_t length;
    uint16_t nextEventId;
    event * events;
    uint16_t addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2);
    void clear();
    uint32_t getEventTimestamp(uint16_t eventIndex);
    //event * getNextEvent();
    void reset();
    void triggerEvent(uint16_t eventIndex);
    void triggerEvents(uint32_t timestamp);
    void printEventArray(uint8_t lastIndex);
};

int compareEvents(const void *s1, const void *s2);
