#include "sequencer.h"

Transport transport;
IntervalTimer sequencerUpdateTimer;

uint8_t currentProject = 0;
uint8_t currentTrack = 0;
uint8_t currentPattern = 0;
uint8_t currentArrangementPosition = 0;
bool midiThrough =  true;

Arrangement arrangement;
Pattern patterns[NR_PATTERNS];
MIDIcallbackGeneric  metronome_midi_cb;
uint8_t metronomeChannel = 10;
uint8_t metronomeNote1 = 76;
uint8_t metronomeNote2 = 77;

String patternNames[NR_PATTERNS] = {"PATRN01", "PATRN02", "PATRN03", "PATRN04", "PATRN05", "PATRN06", "PATRN07", "PATRN08"};
String signatureNames[NR_SIGNATURES] = {"11/4", "7/4", "5/4", "4/4", "2/4", "1/4"};
uint16_t ticksPerBar[NR_SIGNATURES] = {RESOLUTION * 11, RESOLUTION * 7, RESOLUTION * 5, RESOLUTION * 4, RESOLUTION * 2, RESOLUTION};
uint16_t ticksPerBeat[NR_SIGNATURES] = {RESOLUTION, RESOLUTION, RESOLUTION, RESOLUTION, RESOLUTION, RESOLUTION};

event * eventClipboard = nullptr;
uint16_t clipboardNrEvents = 0;

void setupSequencer()
{
  setBpm(transport.bpm);
  metronome_midi_cb = serialMidiSend;
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    String name = patternNames[patternId];
    name.toCharArray(patterns[patternId].name, 8);
  }
   Serial.println(sizeof(event));
}

void updateSequencer()
{
  if (transport.arrangementOn) updateTransport(arrangement.arrangementTick);
  else updateTransport(patterns[currentPattern].patternTick);
  if (transport.metronomeOn && transport.recording) updateMetronome(false);
}

void tickPattern()
{
  if (transport.state == SEQ_PRECOUNT) handlePrecount();
  if (transport.state == SEQ_PLAYING && transport.arrangementOn)
  {
    static uint8_t lastArrPosition = 0;
    currentArrangementPosition = arrangement.tick();
    if (lastArrPosition != currentArrangementPosition)
    {
      lastArrPosition = currentArrangementPosition;
      patterns[currentPattern].reset();
      setMuteStatus();
    }
    currentPattern = arrangement.arrangementItems_a[currentArrangementPosition].patternIndex;
  }
  if (transport.state == SEQ_PLAYING) patterns[currentPattern].tick();
}

void setMuteStatus()
{
  for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
  {
    bool muteStatus =  arrangement.arrangementItems_a[currentArrangementPosition].muteArray[trackId];
    patterns[currentPattern].setMuteStatus(trackId, muteStatus);  
  }
}

void play()
{
  if (transport.recording) transport.state = SEQ_PRECOUNT;
  else transport.state = SEQ_PLAYING;
}

void stop()
{
  transport.state = SEQ_STOPPED;
  transport.precountBars = 0;
  transport.precountTicks = 0;
  updateMetronome(true);
}

void seq_continue()
{
  if (transport.arrangementOn)
  {
    // continue from selected/current arr item
    arrangement.arrangementTick = arrangement.arrangementItems_a[currentArrangementPosition].startTick; 
    if (transport.recording) transport.state = SEQ_PRECOUNT;
    else transport.state = SEQ_PLAYING;
  }
  else play();
}

void reset()
{ 
  if (transport.arrangementOn)
  {
    arrangement.arrangementTick = 0;
    for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++) patterns[patternId].reset();
  }
  else patterns[currentPattern].reset();
  setMuteStatus();
}

void panic() { allNotesOff(); }

void record(bool record)
{
  transport.recording = record;
  if (!record) updateFreeMemory();
}

void handlePrecount()
{
  transport.precountTicks++;
  if (transport.precountTicks / RESOLUTION > transport.precountBars) //beatsPrecounted)
  {
    transport.precountBars++;
    metronome_midi_cb(metronomeChannel, usbMIDI.NoteOn, metronomeNote1, 80);
  }
  if (transport.precountTicks % RESOLUTION > RESOLUTION >> 1 ) metronome_midi_cb(metronomeChannel, usbMIDI.NoteOff, metronomeNote1, 0);
  if (transport.precountBars > 4) // beatsPrecounted > 4)
  {
    metronome_midi_cb(metronomeChannel, usbMIDI.NoteOff, metronomeNote1, 0);
    transport.precountBars = 0;
    transport.precountTicks = 0;
    transport.state = SEQ_PLAYING;
  }
}

void setBpm(uint8_t bpm)
{
  transport.bpm = bpm;
  uint16_t oneTickUs = 1000 * 60000 / (transport.bpm * RESOLUTION);
  sequencerUpdateTimer.begin(tickPattern, oneTickUs); 
}

void processInput(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (patterns[currentPattern].tracks[currentTrack].midi_cb && midiThrough)
  {
    // Midi through
    uint8_t _data1 = data1;
    if (type == usbMIDI.NoteOn || type == usbMIDI.NoteOff) _data1 = _data1 + patterns[currentPattern].tracks[currentTrack].config.transpose;
    patterns[currentPattern].tracks[currentTrack].midi_cb(patterns[currentPattern].tracks[currentTrack].config.channel, type, data1, data2);
  }
  uint32_t timestamp = patterns[currentPattern].patternTick;
  if (transport.recording)
  {
    if (transport.state == SEQ_PLAYING) patterns[currentPattern].tracks[currentTrack].addEvent(timestamp, type, data1, data2);
    if (transport.state == SEQ_PRECOUNT) patterns[currentPattern].tracks[currentTrack].addEvent(0, type, data1, data2);  
  }
  patterns[currentPattern].setActivity(currentTrack, 20);
}

void updateTransport(uint32_t tick)
{
  transport.trp_bar = (uint16_t)(tick / transport.ticksPerBar);
  tick = (uint16_t)(tick % transport.ticksPerBar);
  transport.trp_4th = (uint16_t)(tick / transport.ticksPerBeat);
  tick = (uint16_t)(tick % transport.ticksPerBeat);
  transport.trp_16th = (uint16_t)(tick / (RESOLUTION / 4));
}

void updateMetronome(bool reset)
{
  static int last_4th = -1;
  uint8_t note = 64;
  elapsedMillis timer = 0;
  bool played = false;

  if (reset)
  {
    timer = 51;
    last_4th = -1;
  }
  
  if (last_4th != transport.trp_4th)
  {
    last_4th = transport.trp_4th;
    if (transport.trp_4th == 0) note = metronomeNote1;
    else note = metronomeNote2;
    metronome_midi_cb(metronomeChannel, usbMIDI.NoteOn, note, 80);
    timer = 0;
    played = true;
  }
  
  if (timer > 50 && played)
  {
    played = false;
    metronome_midi_cb(metronomeChannel, usbMIDI.NoteOff, note, 0);
  }
}

void updateFreeMemory()
{
  uint16_t freeMemory = MEMORY_MAX;
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++)
    {
      freeMemory = freeMemory - patterns[patternId].tracks[trackId].memUsage;
    }
  }
  transport.freeMemory = freeMemory;
}

void clearTrack(uint8_t trackId) { patterns[currentPattern].tracks[trackId].clear(); }
void clearTracks() { for (uint8_t trackId = 0; trackId < NR_TRACKS; trackId++) patterns[currentPattern].tracks[trackId].clear(); }
void clearPatterns() { for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++) patterns[patternId].clear(); }
void clearArrangement() { arrangement.clear(); }

void copyTrack()
{
  eventClipboard = patterns[currentPattern].tracks[currentTrack].copy();
  clipboardNrEvents = patterns[currentPattern].tracks[currentTrack].getNrEvents();
}

void pasteTrack()
{
  if (eventClipboard && (clipboardNrEvents > 0)) patterns[currentPattern].tracks[currentTrack].paste(eventClipboard, clipboardNrEvents);
}

void setQuantize(uint8_t quantize) { patterns[currentPattern].tracks[currentTrack].quantize = quantize; }
uint8_t getQuantize() { return patterns[currentPattern].tracks[currentTrack].quantize;}

void setSignature(uint8_t signatureId)
{
  transport.signature = signatureNames[signatureId];
  transport.ticksPerBar= ticksPerBar[signatureId];
  transport.ticksPerBeat= ticksPerBeat[signatureId];

}
