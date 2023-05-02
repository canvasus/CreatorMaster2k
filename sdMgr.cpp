#include "sdMgr.h"

const int chipSelect = BUILTIN_SDCARD;
bool sdStatus = false;
uint8_t currentProjectId = 0;

void initSDcard()
{
  if (!SD.begin(chipSelect)) { Serial.println("Card failed, or not present"); }
  else
  {
    for (uint8_t projectId = 0; projectId < NR_PROJECTS; projectId++) setProjectfolder(projectId);
    sdStatus = true;
    listContent();
  }
 
}

void loadProject()
{
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    loadTrackEvents(patternId);
  }
}

void saveProject()
{
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    saveTrackEvents(patternId);
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
      if (memBlocks > 0)
      {
        event * loadedEvents = (event*)malloc(memBlocks * NR_EVENTS * sizeof(event));
        dataFile.read((uint8_t *)loadedEvents, memBlocks * NR_EVENTS * sizeof(event));
        patterns[patternNr].tracks[trackNr].paste(loadedEvents, memBlocks * NR_EVENTS);
        free(loadedEvents);
      }
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
    if (memBlocks > 0) dataFile.write((uint8_t *)trackEvents, memBlocks * NR_EVENTS * sizeof(event));
  }
  dataFile.close();
}

bool loadSystemSettings()
{
  return true;
}

void loadTrackSettings(uint8_t patternNr)
{
  
}

void saveTrackSettings(uint8_t patternNr)
{
  
}

void loadArrangementSettings()
{
  
}

void saveArrangementSettings()
{
  Serial.println("SAVE data for arrangement");
  char fileName[12] = "CM2K_ARR";
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  for (uint8_t arrItem = 0; arrItem < NR_ARRITEMS; arrItem++)
  {
    dataFile.write((uint8_t *)&arrangement.arrangementItems_a[arrItem], sizeof(ArrangementItem));
  }
  dataFile.close();
}

void setProjectfolder(uint8_t projectId)
{
  SD.sdfs.chdir("/");
  char fileName[15];
  sprintf(fileName, "CM2K_PROJECT_%02d", projectId);
  if (!SD.exists(fileName)) SD.mkdir(fileName);
  SD.sdfs.chdir(fileName);
  //currentProjectFolder = fileName;
  currentProjectId = projectId;
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
    }
    else
    {
      Serial.print("  ");
      Serial.print(entry.name());
      Serial.print("  ");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
