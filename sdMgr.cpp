#include "sdMgr.h"

const int chipSelect = BUILTIN_SDCARD;
bool sdStatus = false;

void initSDcard()
{
  if (!SD.begin(chipSelect)) { Serial.println("Card failed, or not present"); }
  else
  {
    sdStatus = true;
    listContent();
  }
}

void loadTrackEvents(uint8_t patternNr)
{
  Serial.printf("LOAD data for pattern %d\n", patternNr);
  char fileName[18];
  //uint8_t trackNr = 0;
  uint16_t memBlocks = 0;
  sprintf(fileName, "CM2K_PATTERN_%02d", patternNr);
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
    {
      dataFile.read((uint8_t *)&trackNr, sizeof(trackNr));
      dataFile.read((uint8_t *)&memBlocks, sizeof(memBlocks));
      event * loadedEvents = (event*)malloc(memBlocks * NR_EVENTS * sizeof(event));
      dataFile.read((uint8_t *)loadedEvents, memBlocks * NR_EVENTS * sizeof(event));
      patterns[patternNr].tracks[trackNr].paste(loadedEvents, memBlocks * NR_EVENTS);
      free(loadedEvents);
    }
  }
}

void saveTrackEvents(uint8_t patternNr)
{
  Serial.printf("SAVE data for pattern %d\n", patternNr);
  // save all event data from used tracks
  char fileName[18];
  sprintf(fileName, "CM2K_PATTERN_%02d", patternNr);
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
  {
    uint16_t memBlocks = patterns[patternNr].tracks[trackNr].memBlocks;
    event * trackEvents = patterns[patternNr].tracks[trackNr].copy();
    dataFile.write((uint8_t *)&trackNr, sizeof(trackNr));
    dataFile.write((uint8_t *)&memBlocks, sizeof(memBlocks));
    dataFile.write((uint8_t *)trackEvents, memBlocks * NR_EVENTS * sizeof(event));
  }
  dataFile.close();
}


void listContent()
{
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break; // no more files
    if (entry.isDirectory())
    {
      Serial.print(entry.name());
      Serial.println("/");
      //if (!SD.rmdir(entry.name())) Serial.println("unable to delete dir");
    }
    else
    {
      Serial.print("  ");
      Serial.print(entry.name());
      Serial.print("  ");
      Serial.println(entry.size(), DEC);
      //if (!SD.remove(entry.name())) Serial.println("unable to delete FILE");
      
    }
    entry.close();
  }
}
