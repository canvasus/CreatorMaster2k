#include "peripherals.h"
#include "sequencer.h"

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi1(myusb);
KeyboardController keyboard1(myusb);
USBHIDParser hid1(myusb);
MouseController mouse1(myusb);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setupPeripherals()
{
  Serial.println(F("MIDI SETUP"));
  delay(100);
  myusb.begin();
  delay(200);
  clearUsbMidi();
  midi1.setHandleNoteOn(input_noteOn);
  midi1.setHandleNoteOff(input_noteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void updateMidi()
{
  myusb.Task();
  midi1.read();
}

void input_noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  Serial.printf("Note on, ch: %d, note: %d\n", channel, note);
  processNoteOn(channel, note, velocity);
}

void input_noteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  Serial.printf("Note off, ch: %d, note: %d\n", channel, note);
  processNoteOff(channel, note, velocity);
}

void input_controlChange(uint8_t channel, uint8_t control, uint8_t value)
{
  Serial.printf("CC, ch: %d, control: %d, value: %d\n", channel, control, value);
}

void serialMidiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) { MIDI.sendNoteOn(note, velocity, channel); }
void serialMidiNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) { MIDI.sendNoteOff(note, velocity, channel); }

void allNotesOff()
{
  for (uint8_t channel = 1; channel < 17; channel++)
  {
    //for (uint8_t note = 0; note <= 127; note++) MIDI.sendNoteOff(note, 0, channel);
    //delay(1);
    MIDI.sendControlChange(120, 0, channel);
    MIDI.sendControlChange(123, 0, channel);
  }
}


void clearUsbMidi()
{
  for (uint8_t i = 0; i < 25; i++) midi1.read();
  delay(10);
}
