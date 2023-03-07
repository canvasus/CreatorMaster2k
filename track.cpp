#include "track.h"

Track::Track()
{
  events = (event*)malloc(NR_EVENTS * sizeof(event));
  memset(events, 0, NR_EVENTS * sizeof(event));
  noteOn_cb = nullptr;
  noteOff_cb = nullptr;
  channel = 1;
  quantize = 0;
}

void Track::setHandleNoteOn(MIDIcallback cb) { noteOn_cb = cb; }
void Track::setHandleNoteOff(MIDIcallback cb) { noteOff_cb = cb; }

void Track::triggerEvent(uint16_t eventIndex)
{
  if (eventIndex < NR_EVENTS)
  {
    //Serial.printf("triggered event %d\n", eventIndex);
    switch (events[eventIndex].type)
    {
      case usbMIDI.NoteOn:
        if (noteOn_cb) noteOn_cb(channel, events[eventIndex].data1, events[eventIndex].data2);
        break;
      case usbMIDI.NoteOff:
        if (noteOff_cb) noteOff_cb(channel, events[eventIndex].data1, events[eventIndex].data2);
        break;
    }
  }
}

 void Track::triggerEvents(uint16_t timestamp)
 {
   //timestamp = timestamp + RESOLUTION / quantize; // timestamp + 1/2 quantize step length
   if (_quantizeCounter == 0)
   {
     _quantizeCounter = quantize;
     while ( (nextEventId < NR_EVENTS) && (events[nextEventId].timestamp <= timestamp) && (events[nextEventId].type != NONE) )
     {
        triggerEvent(nextEventId++);
     }
   }
   else _quantizeCounter--;
 }

void Track::reset() { nextEventId = 0; }

uint16_t Track::addEvent(uint16_t timestamp, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (_nrEvents < NR_EVENTS - 1)
  {
    events[NR_EVENTS - 1].timestamp = timestamp;
    events[NR_EVENTS - 1].type = type;
    events[NR_EVENTS - 1].data1 = data1;
    events[NR_EVENTS - 1].data2 = data2;
    _nrEvents++;
    _sortEvents();
    //printEventArray(32);
    return _nrEvents;
  }
  else return 0;
}

void Track::clear()
{
  memset(events, 0, NR_EVENTS * sizeof(event));
  _nrEvents = 0;
}

void Track::_sortEvents() { qsort(events, NR_EVENTS, sizeof(event), compareEvents); }

uint16_t Track::getEventTimestamp(uint16_t eventIndex) { return events[eventIndex].timestamp; }

void Track::printEventArray(uint8_t lastIndex)
{
  Serial.print("_nrEvents: ");
  Serial.println(_nrEvents);
  
  Serial.print("Types: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(events[i].type);
    Serial.print(", ");
  }
  Serial.println("END");
  
  Serial.print("Times: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(events[i].timestamp);
    Serial.print(", ");
  }
  Serial.println("END");

  Serial.print("Data1: ");
  for (uint8_t i = 0; i < lastIndex; i++)
  {
    Serial.print(events[i].data1);
    Serial.print(", ");
  }
  Serial.println("END");
}


int compareEvents(const void *s1, const void *s2)
{
    struct event *e1 = (struct event *)s1;
    struct event *e2 = (struct event *)s2;
    // Active events first in array
    if(e1->type == NONE && e2->type != NONE) return 1;
    if(e1->type != NONE && e2->type == NONE) return -1;
    // Then sort by tick
    if(e1->timestamp < e2->timestamp) return -1;
    if(e1->timestamp > e2->timestamp) return 1;
    return 0;
}
