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
  Serial.print(F("MIDI SETUP..."));
  delay(100);
  myusb.begin();
  delay(200);
  clearUsbMidi();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  Serial.println(F("DONE"));
}

void updateMidi()
{
  myusb.Task();
  if (midi1.read()) processInput(midi1.getChannel(),midi1.getType(), midi1.getData1(), midi1.getData2());
  if (MIDI.read()) processInput(MIDI.getChannel(),MIDI.getType(), MIDI.getData1(), MIDI.getData2());
}

void serialMidiSend(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  midi::MidiType mtype = (midi::MidiType)type;
  MIDI.send(mtype, data1, data2, channel);
}

void allNotesOff()
{
  for (uint8_t channel = 1; channel < 17; channel++)
  {
    MIDI.sendControlChange(120, 0, channel);
    MIDI.sendControlChange(123, 0, channel);
  }
}

void clearUsbMidi()
{
  //for (uint8_t i = 0; i < 25; i++) midi1.read();
  //delay(10);
}
