#include "ui.h"
#include "peripherals.h"
#include "sequencer.h"
#include "sdMgr.h"

void setup()
{
  Serial.println(F("START"));
  if (CrashReport) Serial.print(CrashReport);
  setupPeripherals();
  initSDcard();
  setupUI();
  setupSequencer();
  
}

void loop()
{
 updateMidi();
 updateSequencer();
 updateUI();
}
