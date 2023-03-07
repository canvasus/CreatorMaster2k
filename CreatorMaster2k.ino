#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"

void setup()
{
  Serial.println(F("START"));
  if (CrashReport) Serial.print(CrashReport);
  setupPeripherals();
  setupUI();
  setupSequencer();
}

void loop()
{
 updateMidi();
 updateSequencer();
 updateUI();
}
