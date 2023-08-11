#include <SD.h>
#include "src/MTP_Teensy/MTP_Teensy.h"

#define CS_SD BUILTIN_SDCARD

void setup()
{
  MTP.begin();
  SD.begin(CS_SD);
  MTP.addFilesystem(SD, "SD Card");
}

void loop() {
  MTP.loop();
}
