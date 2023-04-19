#pragma once

#include <SD.h>
#include "sequencer.h"
#include "x_globals.h"

void initSDcard();
void listContent();

void saveTrackEvents(uint8_t patternNr);
void loadTrackEvents(uint8_t patternNr);
