#include "peripherals.h"
#include "sequencer.h"

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi1(myusb);
MIDIDevice midi2(myusb);
MIDIDevice midi3(myusb);
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
  if (midi2.read()) processInput(midi2.getChannel(),midi2.getType(), midi2.getData1(), midi2.getData2());
  if (midi3.read()) processInput(midi3.getChannel(),midi3.getType(), midi3.getData1(), midi3.getData2());
  if (MIDI.read()) processInput(MIDI.getChannel(),MIDI.getType(), MIDI.getData1(), MIDI.getData2());
}

void serialMidiSend(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  //Serial.printf("Send (serial): type %d, data1 %d, data2 %d\n", type, data1, data2);
  midi::MidiType mtype = (midi::MidiType)type;
  MIDI.send(mtype, data1, data2, channel);
}

void usbMidi1Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  //Serial.printf("Send (midi1): type %d, data1 %d, data2 %d\n", type, data1, data2);
  midi1.send(type, data1, data2, channel);
  //midi1.send_now();
}
void usbMidi2Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  //Serial.printf("Send (midi2): type %d, data1 %d, data2 %d\n", type, data1, data2);
  if (midi2) midi2.send(type, data1, data2, channel);
  //Serial.println("done");
  //if (midi2) midi2.send_now();
}

void usbMidi3Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  midi3.send(type, data1, data2, channel);
  //midi3.send_now();
}

void sendStart()
{
  if (midi2)
  {
    midi2.sendRealTime(usbMIDI.Start);
    //midi2.send_now();
  }
}

void sendClock()
{
  if (midi2)
  {
    midi2.sendRealTime(usbMIDI.Clock);
    //midi2.send_now();
  }
}

void sendStop()
{
  if (midi2)
  {
    midi2.sendRealTime(usbMIDI.Stop);
    //midi2.send_now();
  }
}

void allNotesOff()
{
  for (uint8_t channel = 1; channel < 17; channel++)
  {
    MIDI.sendControlChange(120, 0, channel);
    MIDI.sendControlChange(123, 0, channel);
    midi1.sendControlChange(123, 0, channel);
    midi2.sendControlChange(123, 0, channel);
    midi3.sendControlChange(123, 0, channel);
  }
}

void clearUsbMidi()
{
  //for (uint8_t i = 0; i < 25; i++) midi1.read();
  //delay(10);
}
