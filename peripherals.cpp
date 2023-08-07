#include "peripherals.h"
#include "sequencer.h"

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice_BigBuffer midi1(myusb);
MIDIDevice_BigBuffer midi2(myusb);
MIDIDevice_BigBuffer midi3(myusb);
USBHIDParser hid1(myusb);
MouseController mouse1(myusb);

MIDIDevice_BigBuffer * midiDrivers[3] = {&midi1, &midi2, &midi3};
String driver_names[3] = {"midi1", "midi2", "midi3"};
//#define CNT_DEVICES (sizeof(midiDrivers)/sizeof(midiDrivers[0])) 

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

SystemConfig systemConfig;

midiEvent messageBuffer2[64];
uint16_t nrBufferedMessages2 = 0;
bool sendClockRateLimited = false;

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
  updateRateLimitSenders();
}

void updateRateLimitSenders()
{
  static elapsedMicros timer = 0;
  
  // NOTE: testing for midi2 mainly

  if ((timer > systemConfig.rateLimitsUs[1]) && sendClockRateLimited )
  {
    timer = 0;
    midi2.sendRealTime(usbMIDI.Clock);
    sendClockRateLimited = false;
    return;
  }

  if ((timer > systemConfig.rateLimitsUs[1]) && (nrBufferedMessages2 > 0) )
  {
    timer = 0;
    midi2.send(messageBuffer2[0].type, messageBuffer2[0].data1, messageBuffer2[0].data2, messageBuffer2[0].channel);
    midi2.send_now();
    for (uint8_t index = 0; index < nrBufferedMessages2; index++)
    {
      messageBuffer2[index] = messageBuffer2[index +1];
    }
    nrBufferedMessages2--;
  }
}

void addRateLimitMessage2(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (nrBufferedMessages2 < 64)
  {
    messageBuffer2[nrBufferedMessages2].channel = channel;
    messageBuffer2[nrBufferedMessages2].type = type;
    messageBuffer2[nrBufferedMessages2].data1 = data1;
    messageBuffer2[nrBufferedMessages2].data2 = data2;
    nrBufferedMessages2++;
  }
}

void serialMidiSend(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  midi::MidiType mtype = (midi::MidiType)type;
  MIDI.send(mtype, data1, data2, channel);
}

void usbMidi1Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  //Serial.printf("Send (midi1): type %d, data1 %d, data2 %d\n", type, data1, data2);
  if (midi1) midi1.send(type, data1, data2, channel);
  if (midi1) midi1.send_now();
}
void usbMidi2Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  //Serial.printf("Send (midi2): type %d, data1 %d, data2 %d\n", type, data1, data2);
  addRateLimitMessage2(channel, type, data1, data2);
  //if (midi2) midi2.send(type, data1, data2, channel);
  //if (midi2) midi2.send_now();
}

void usbMidi3Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2)
{
  if (midi3) midi3.send(type, data1, data2, channel);
  //midi3.send_now();
}

void sendStart()
{
  if (midi2)
  {
    midi2.sendRealTime(usbMIDI.Start);
    midi2.send_now();
  }
}

void sendClock()
{
  if (midi2)
  {
    sendClockRateLimited = true;
    //midi2.sendRealTime(usbMIDI.Clock);
    //midi2.send_now();
  }
}

void sendStop()
{
  if (midi2)
  {
    midi2.sendRealTime(usbMIDI.Stop);
    midi2.send_now();
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

// String getUsbDeviceName(uint8_t deviceIndex)
// {
//   String name = "";
//   if (midiDrivers[deviceIndex])
//   {
//     const uint8_t * productName = midiDrivers[deviceIndex]->product();
//     char name[16];
//     snprintf(name, 16, "%s", productName);
//   }
//   return name;  
// }

void getUsbDeviceName(uint8_t usbIndex, char * buf, uint8_t maxBufferSize)
{
  char na[4] = {'N', '/', 'A'};
  if (*midiDrivers[usbIndex])
  {
    const uint8_t * productName = midiDrivers[usbIndex]->product();
    for(uint8_t i = 0; i < maxBufferSize; i++) buf[i] = productName[i];
  }
  else
  {
    for(uint8_t i = 0; i < 4; i++) buf[i] = na[i];
  }
}