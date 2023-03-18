#include "track.h"

Track::Track()
{
  events = nullptr;
  noteOn_cb = nullptr;
  noteOff_cb = nullptr;
  channel = 1;
  name = "<empty>";
}

void Track::_initBuffer()
{
  events = (event*)malloc(NR_EVENTS * sizeof(event));
  memset(events, 0, NR_EVENTS * sizeof(event));
  memUsage = NR_EVENTS;
}

void Track::_releaseBuffer()
{
  free(events);
  events = nullptr;
  memUsage = 0;
}

void Track::setHandleNoteOn(MIDIcallback cb) { noteOn_cb = cb; }
void Track::setHandleNoteOff(MIDIcallback cb) { noteOff_cb = cb; }

void Track::triggerEvent(uint16_t eventIndex)
{
  if (events != nullptr && eventIndex < _nrEvents)
  {
    switch (events[eventIndex].type)
    {
      case usbMIDI.NoteOn:
        if (noteOn_cb) noteOn_cb(channel, events[eventIndex].data1 + transpose, events[eventIndex].data2);
        break;
      case usbMIDI.NoteOff:
        if (noteOff_cb) noteOff_cb(channel, events[eventIndex].data1 + transpose, events[eventIndex].data2);
        break;
    }
  }
}

 uint16_t Track::triggerEvents(uint32_t timestamp)
 {
  uint16_t eventCounter = 0;

  if (hidden) return 0;
  
  if(loop > 0)
  {
    timestamp = timestamp % (loop * 192); // loop is set in 1/4ths
    if (timestamp == 0) nextEventId = 0;
  }
  
  if (events != nullptr)
  {
    while ( (nextEventId < _nrEvents) && (events[nextEventId].type != NONE) )
    {
      uint32_t eventTimestamp = events[nextEventId].timestamp;
      uint8_t eventType = events[nextEventId].type;
      bool trigger = false;
      
      if ( (eventType == usbMIDI.NoteOn) && (_quantizeTimestamp(eventTimestamp) <= timestamp)) trigger = true;
      if ( (eventType != usbMIDI.NoteOn) && (eventTimestamp <= timestamp)) trigger = true;
  
      if (trigger)
      {
        triggerEvent(nextEventId++);
        eventCounter++;
      }
      else break;
    }
   } // IF != nullptr
   return eventCounter;
}

uint32_t Track::_quantizeTimestamp(uint32_t timestamp) { return ((timestamp + (quantize>>1)) / quantize) * quantize; }

void Track::reset()
{
  if (events != nullptr) _convertTempEvents(); 
  nextEventId = 0;
  _loopCounter = 0;
}

void Track::_convertTempEvents()
{
  for (uint16_t eventId = 0; eventId < _nrEvents; eventId++)
  {
    if (events[eventId].type == TYPE_NOTEON_TEMP) events[eventId].type = usbMIDI.NoteOn;
    if (events[eventId].type == TYPE_NOTEOFF_TEMP) events[eventId].type = usbMIDI.NoteOff;
  }
}

uint16_t Track::addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (_nrEvents == 0 || events == nullptr) _initBuffer();
  
  if (_nrEvents < NR_EVENTS - 1)
  {
    events[_nrEvents].timestamp = timestamp;
    events[_nrEvents].type = type - 1;
    events[_nrEvents].data1 = data1;
    events[_nrEvents].data2 = data2;
    _nrEvents++;
    _sortEvents();
    //printEventArray(16);
    return _nrEvents;
  }
  else return 0;
}

void Track::clear()
{
  _releaseBuffer();
  _nrEvents = 0;
}

void Track::_sortEvents() { qsort(events, _nrEvents, sizeof(event), compareEvents); }

uint32_t Track::getEventTimestamp(uint16_t eventIndex)
{
  if ( (events != nullptr) && (eventIndex < _nrEvents)) return events[eventIndex].timestamp;
  else return 4294967295;
}

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
