#pragma once

#include <SD.h>
#include "sequencer.h"
#include "track.h"
#include "pattern.h"
#include "arrangement.h"
#include "x_globals.h"

void initSDcard();
void listContent();

bool loadSystemSettings();

void setProjectfolder(uint8_t projectId);

void loadProject();
void saveProject();

void saveTrackEvents(uint8_t patternNr);
void loadTrackEvents(uint8_t patternNr);
void loadTrackSettings(uint8_t patternNr);
void saveTrackSettings(uint8_t patternNr);
void loadArrangementSettings(uint8_t arrNr);
void saveArrangementSettings(uint8_t arrNr);