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
    setProjectfolder(0);
  }
}

void loadProject()
{
  loadPatterns();
  loadArrangementSettings();
  loadTransport();
}

void loadPatterns()
{
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    loadTrackEvents(patternId);
    loadTrackSettings(patternId);
  }
}

void saveProject()
{
  for (uint8_t patternId = 0; patternId < NR_PATTERNS; patternId++)
  {
    saveTrackEvents(patternId);
    saveTrackSettings(patternId);
  }
  saveArrangementSettings();
  saveTransport();
}

void loadTrackEvents(uint8_t patternNr)
{
  char fileName[18];
  sprintf(fileName, "CM2K_PATTERN_%03d", patternNr);
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
    {
      uint16_t nrEvents = 0;
      dataFile.read((uint8_t *)&trackNr, sizeof(trackNr));
      dataFile.read((uint8_t *)&nrEvents, sizeof(nrEvents));
      if (nrEvents > 0)
      {
        event * loadedEvents = (event*)malloc(nrEvents * sizeof(event));
        memset(loadedEvents, 0, nrEvents * sizeof(event));
        dataFile.read((uint8_t *)loadedEvents, nrEvents * sizeof(event));
        patterns[patternNr].tracks[trackNr].paste(loadedEvents, nrEvents);
        //Serial.printf("Pattern %d, Track %d, loaded %d events\n", patternNr, trackNr, nrEvents);
        free(loadedEvents);
      }
      else patterns[patternNr].tracks[trackNr].clear();
    }
    dataFile.close();
  }
}

uint16_t getNrEvents(event * eventBuffer, uint16_t bufferSizeBlocks)
{
  uint16_t nrEvents = 0;
  for (uint16_t eventId = 0; eventId < bufferSizeBlocks * NR_EVENTS; eventId++)
  {
    if (eventBuffer[eventId].type != 0) nrEvents++;
  }
  return nrEvents;
}

void saveTrackEvents(uint8_t patternNr)
{
  char fileName[18];
  sprintf(fileName, "CM2K_PATTERN_%03d", patternNr);
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  for (uint8_t trackNr = 0; trackNr < NR_TRACKS; trackNr++)
  {
    uint16_t nrEvents = patterns[patternNr].tracks[trackNr]._nrEvents;
    event * trackEvents = patterns[patternNr].tracks[trackNr].copy();
    dataFile.write((uint8_t *)&trackNr, sizeof(trackNr));
    dataFile.write((uint8_t *)&nrEvents, sizeof(nrEvents));
    if (nrEvents > 0) dataFile.write((uint8_t *)trackEvents, nrEvents * sizeof(event));
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
      patterns[patternNr].tracks[trackNr].syncSettings();
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
    dataFile.close();
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

void loadTransport()
{
  const char fileName[15] = "CM2K_TRANSPORT";
  if (SD.exists(fileName))
  {
    File dataFile = SD.open(fileName, FILE_READ);
    dataFile.read((uint8_t *)&transport, sizeof(Transport));
    dataFile.close();
  }
  syncTransportSettings();
}

void saveTransport()
{
  const char fileName[15] = "CM2K_TRANSPORT";
  SD.remove(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  dataFile.write((uint8_t *)&transport, sizeof(Transport));
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
