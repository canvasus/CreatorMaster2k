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
    loadTrackSettings(patternId);
  }
  loadArrangementSettings();
}

void saveProject()
{
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    saveTrackEvents(patternId);
    saveTrackSettings(patternId);
  }
  saveArrangementSettings();
}

void loadTrackEvents(uint8_t patternNr)
{
  char fileName[18];
  uint16_t memBlocks = 0;
  sprintf(fileName, "CM2K_PATTERN_%03d", patternNr);
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
      else patterns[patternNr].tracks[trackNr].clear();
    }
  }
}

void saveTrackEvents(uint8_t patternNr)
{
  char fileName[18];
  sprintf(fileName, "CM2K_PATTERN_%03d", patternNr);
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
  char fileName[20];
  sprintf(fileName, "CM2K_PATTERNSET_%03d", patternNr);
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
    {
      dataFile.read((uint8_t *)&patterns[patternNr].tracks[trackNr].config, sizeof(TrackConfig));
    }
    dataFile.close();
  }
}

void saveTrackSettings(uint8_t patternNr)
{
  char fileName[20];
  sprintf(fileName, "CM2K_PATTERNSET_%03d", patternNr);
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
  {
    dataFile.write((uint8_t *)&patterns[patternNr].tracks[trackNr].config, sizeof(TrackConfig));
  }
  dataFile.close();
}

void loadArrangementSettings()
{
  const char fileName[9] = "CM2K_ARR";
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    for (uint8_t arrItem = 0; arrItem < NR_ARRITEMS; arrItem++)
    {
      dataFile.read((uint8_t *)&arrangement.arrangementItems_a[arrItem], sizeof(ArrangementItem));
    }
  }
}

void saveArrangementSettings()
{
  const char fileName[9] = "CM2K_ARR";
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
  char fileName[17];
  sprintf(fileName, "CM2K_PROJECT_%03d", projectId);
  if (!SD.exists(fileName)) SD.mkdir(fileName);
  SD.sdfs.chdir(fileName);
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
