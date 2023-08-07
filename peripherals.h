#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>
#include "x_globals.h"


struct midiEvent
{
  uint8_t   type = 0;
  uint8_t   data1 = 0;
  uint8_t   data2 = 0;
  uint8_t   channel = 0;
};

struct SystemConfig
{
  uint16_t rateLimitsUs[NR_USBPORTS] = {0,500,0};
  bool    sendClock[NR_USBPORTS] = {false, true, false};
  uint8_t metronomeChannel = 10;
  uint8_t metronomeNote1 = 48;
  uint8_t metronomeNote2 = 56;
};

extern SystemConfig systemConfig;

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

void updateRateLimitSenders();

//String getUsbDeviceName(uint8_t deviceIndex);
void getUsbDeviceName(uint8_t usbIndex, char * buf, uint8_t maxBufferSize);