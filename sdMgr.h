#pragma once

#include <SD.h>
#include "sequencer.h"
#include "track.h"
#include "pattern.h"
#include "arrangement.h"
#include "x_globals.h"

void initSDcard();
void listContent();

void loadSystemSettings();
void saveSystemSettings();

void setProjectfolder(uint8_t projectId);

void loadProject();
void saveProject();

void loadPatterns();

void saveTrackEvents(uint8_t patternNr);
void loadTrackEvents(uint8_t patternNr);
void loadTrackSettings(uint8_t patternNr);
void saveTrackSettings(uint8_t patternNr);
void loadArrangementSettings();
void saveArrangementSettings();
void loadTransport();
void saveTransport();

uint16_t getNrEvents(event * eventBuffer, uint16_t bufferSizeBlocks);
