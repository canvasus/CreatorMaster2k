#include <SD.h>
#include "src/MTP_Teensy/MTP_Teensy.h"

#define CS_SD BUILTIN_SDCARD  // Works on T_3.6 and T_4.1

void setup()
{
  // mandatory to begin the MTP session.
  MTP.begin();

  // Add SD Card
  SD.begin(CS_SD);
  MTP.addFilesystem(SD, "SD Card");
}

void loop() {
  MTP.loop();  //This is mandatory to be placed in the loop code.
}
