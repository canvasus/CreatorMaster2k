#pragma once

#include <MIDI.h>
#include <USBHost_t36.h>

void setupPeripherals();
void updateMidi();

void input_noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void input_noteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void input_controlChange(uint8_t channel, uint8_t control, uint8_t value);

void serialMidiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void serialMidiNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);

void clearUsbMidi();
