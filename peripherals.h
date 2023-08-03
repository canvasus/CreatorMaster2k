#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>

struct SystemSettings
{
  
};

void setupPeripherals();
void updateMidi();

void serialMidiSend(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);
void usbMidi1Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);
void usbMidi2Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);
void usbMidi3Send(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);

void sendStart();
void sendClock();
void sendStop();

void clearUsbMidi();
void allNotesOff();
