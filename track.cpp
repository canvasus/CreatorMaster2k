#include "track.h"

String emptyName = "<empty>";
String usedName = "In use";

const uint16_t quantizeSettingsT[NR_QUANTIZESTEPS] = {1, 192, 96, 48, 64};
const uint16_t compressSettingsT[NR_COMPRESSTEPS] = {0, 1, 2, 3, 4};  // 63 + abs(velocity - 64) / C

Track::Track()
{
  events = nullptr;
  midi_cb = nullptr;
  config.channel = 1;
  emptyName.toCharArray(config.name, 8);
  memset(_noteStatus, 0, 128);
  _notesPlaying = 0;
  _nrTempEvents = 0;
}

void Track::_initBuffer()
{
  if (events != nullptr) _releaseBuffer();
  events = (event*)malloc(NR_EVENTS * sizeof(event));
  memset(events, 0, NR_EVENTS * sizeof(event));
  memUsage = NR_EVENTS;
  memBlocks = 1;
}

void Track::_releaseBuffer()
{
  free(events);
  events = nullptr;
  memUsage = 0;
  memBlocks = 0;
}

void Track::_expandBuffer()
{
  event * eventsNew = (event*)malloc((memBlocks + 1) * NR_EVENTS * sizeof(event));
  memset(eventsNew, 0, (memBlocks + 1) * NR_EVENTS * sizeof(event));
  memcpy(eventsNew, events, memBlocks * NR_EVENTS * sizeof(event));
  free(events);
  events = eventsNew;
  memBlocks++;
  memUsage = memBlocks * NR_EVENTS;
}

void Track::paste(event * eventClipboard, uint16_t nrEvents)
{
  free(events);
  memBlocks = (nrEvents / NR_EVENTS) + 1;
  memUsage = memBlocks * NR_EVENTS;
  events = (event*)malloc(memBlocks * NR_EVENTS * sizeof(event));
  memcpy(events, eventClipboard, nrEvents * sizeof(event));
  _nrEvents = nrEvents;
  if (nrEvents > 0) usedName.toCharArray(config.name, 8);
  else emptyName.toCharArray(config.name, 8);
}

event * Track::copy() { return events; }

void Track::setMidiCb(MIDIcallbackGeneric cb) { midi_cb = cb; }

void Track::triggerEvent(uint8_t type, uint8_t data1, uint8_t data2)
{
  if (events != nullptr)// && eventIndex < _nrEvents)
  {
    switch (type)
    {
      case usbMIDI.NoteOn:
        data1 = data1 + config.transpose;
        if ( (config.compressIndex > 0) || (config.velocity != 0) ) data2 = _processVelocity(data2);
        _noteStatus[data1] = true;
        _notesPlaying++;
        if (midi_cb) midi_cb(config.channel, type, data1, data2);
        break;
      case usbMIDI.NoteOff:
        data1 = data1 + config.transpose;
        _noteStatus[data1] = false;
        _notesPlaying--;
        if (midi_cb) midi_cb(config.channel, type, data1, data2);
        break;
      default:
        if (midi_cb) midi_cb(config.channel, type, data1, data2);
        break;
    }
  }
}

 uint16_t Track::triggerEvents(uint32_t timestamp)
 {
  uint16_t eventCounter = 0;
  uint32_t inTime = timestamp;

  if (config.hidden) return 0;
  
  // if (config.generatorOn)
  // {
  //   eventCounter = 0;
  //   return eventCounter;
  // }

  if (events != nullptr) // && !config.generatorOn)
  {
    if(config.loop > 0) timestamp = timestamp % (config.loop * 192); // loop is set in 1/4ths

    if (timestamp == 0)
      {
        cleanupNoteOff();
        nextEventId = 0;
      }

    while ( (nextEventId < _nrEvents) && (events[nextEventId].type != NONE) )
      {
        uint32_t eventTimestamp = events[nextEventId].timestamp;
        uint8_t eventType = events[nextEventId].type;
        uint8_t data1 = events[nextEventId].data1;
        uint8_t data2 = events[nextEventId].data2;

        bool trigger = false;
        
        if ( (eventType == usbMIDI.NoteOn) && (_quantizeTimestamp(eventTimestamp) <= timestamp)) trigger = true;
        if ( (eventType != usbMIDI.NoteOn) && (eventTimestamp <= timestamp)) trigger = true;
  
        if (trigger)
        {
          if (eventType == usbMIDI.NoteOn) Serial.printf("Note on. timestampIn %d, eventTime %d, quantizedTime %d\n", inTime, eventTimestamp, _quantizeTimestamp(eventTimestamp));
          triggerEvent(eventType, data1, data2);
          nextEventId++;
          eventCounter++;
        }
        else break;
      }
    }
  return eventCounter;
}

uint32_t Track::_quantizeTimestamp(uint32_t timestamp) { return ((timestamp + (quantize>>1)) / quantize) * quantize; } // returns nearest

uint8_t Track::_processVelocity(uint8_t velocityIn)
{
  int16_t velocity_temp = velocityIn;
  if (config.compressIndex > 0 && config.compressIndex < 4) velocity_temp = 63 + (velocityIn - 63) / compress;
  if (config.compressIndex == 4) velocity_temp = 63; // fix
  if (config.velocity != 0) velocity_temp = velocity_temp + config.velocity;
  uint8_t velocityOut = constrain(velocity_temp, 0, 127);
  return velocityOut;
}

void Track::reset()
{
  if (events != nullptr) _convertTempEvents(); 
  //nextEventId = 0;
  cleanupNoteOff();
}

void Track::setPosition(uint32_t timestamp)
{
  nextEventId = 0;
  while ( (nextEventId < _nrEvents) && (events[nextEventId].type != NONE) )
  {
    if (events[nextEventId].timestamp >= timestamp) break;
    else nextEventId++;
  }
}

void Track::cleanupNoteOff()
{
  for (uint8_t note = 0; note < 128; note++)
  {
    if (_noteStatus[note] && midi_cb)
    {
      midi_cb(config.channel, usbMIDI.NoteOff, note, 0);
      _notesPlaying--;
    }
  }
}

void Track::_convertTempEvents()
{
  if (_nrTempEvents > 0)
  {
    for (uint16_t eventId = 0; eventId < _nrEvents; eventId++)
    {
      if (events[eventId].type == TYPE_NOTEON_TEMP) events[eventId].type = usbMIDI.NoteOn;
      if (events[eventId].type == TYPE_NOTEOFF_TEMP) events[eventId].type = usbMIDI.NoteOff;
      if (events[eventId].type == TYPE_CONTROLCHANGE_TEMP) events[eventId].type = usbMIDI.ControlChange;
      if (events[eventId].type == TYPE_PITCHBEND_TEMP) events[eventId].type = usbMIDI.PitchBend;
    }
    _nrTempEvents = 0;
    _sortEvents();
  }
}

uint16_t Track::addEvent(uint32_t timestamp, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (_nrEvents == 0 || events == nullptr) _initBuffer();
  if (_nrEvents >= (memBlocks * NR_EVENTS - 1)) _expandBuffer();
  
  if (_nrEvents < (memBlocks * NR_EVENTS - 1))
  {
    events[_nrEvents].timestamp = timestamp;
    events[_nrEvents].type = type - 1;
    events[_nrEvents].data1 = data1;
    events[_nrEvents].data2 = data2;
    _nrTempEvents++;
    _nrEvents++;
    //_sortEvents();
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

void Track::syncSettings()
{
  quantize = quantizeSettingsT[config.quantizeIndex];
  compress = compressSettingsT[config.compressIndex];
}

uint16_t Track::getNrEvents() { return _nrEvents; }

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

Generator::Generator()
{
  memset(_matrix, 0, 16*16);
  _currentStep = 0;
  stepLength = RESOLUTION / 4;
  sequenceLength = 16;
  noteLength = stepLength - 1;
}

event Generator::getEvent(uint32_t timestamp)
{
  timestamp = timestamp % (sequenceLength);
  
  if (timestamp == 0) _currentStep = 0;

  event generatorEvent;
  return generatorEvent;
}

void Generator::reset()
{
  _currentStep = 0;
}