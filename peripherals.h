#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>

void setupPeripherals();
void updateMidi();

void serialMidiSend(uint8_t channel, uint8_t type, uint8_t data1, uint8_t data2);
void usbMidiSend(uint8_t usbDeviceNr, uint8_t type, uint8_t data1, uint8_t data2);

void clearUsbMidi();
void allNotesOff();
