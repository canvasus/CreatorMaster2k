#include "arrangement.h"

ArrangementItem::ArrangementItem()
{
  startTick = 0;
  lengthTicks = 4 * 4 * RESOLUTION;
  //startBars = 1;
  //lengthBars = 4;
  patternIndex = 0;
  status = ARRITEM_INACTIVE;
  memset(muteArray, 0, NR_TRACKS);
}

Arrangement::Arrangement()
{
  arrangementItems_a[0].status = ARRITEM_ACTIVE;
  arrangementTick = 0;
}

uint8_t Arrangement::tick()
{
  arrangementTick++;
  return getCurrentArrItem();
}

uint8_t Arrangement::getCurrentArrItem()
{
  //uint16_t currentBar = (arrangementTick / (4 * RESOLUTION - 1)) + 1;
  for (uint8_t itemId = 0; itemId < NR_ARRITEMS; itemId++)
  {
    if ( (arrangementItems_a[itemId].status == ARRITEM_ACTIVE) && 
         (arrangementItems_a[itemId].startTick <= arrangementTick) &&
         ((arrangementItems_a[itemId].startTick + arrangementItems_a[itemId].lengthTicks) > arrangementTick) 
          ) return itemId;
//    if ( (arrangementItems_a[itemId].status == ARRITEM_ACTIVE) && 
//         (arrangementItems_a[itemId].startBars <= currentBar) &&
//         ((arrangementItems_a[itemId].startBars + arrangementItems_a[itemId].lengthBars) > currentBar) 
//          ) return itemId;
  }
  return 0;
}

uint8_t Arrangement::newArrangementItem()
{
  for (uint8_t itemId = 0; itemId < NR_ARRITEMS; itemId++)
  {
    if (arrangementItems_a[itemId].status == ARRITEM_INACTIVE)
    {
      arrangementItems_a[itemId].status = ARRITEM_ACTIVE;
      updateArrangementStartPositions();
      return itemId;
    }
  }
  return 0;
}

uint8_t Arrangement::deleteArrangementItem(uint8_t itemId)
{
  arrangementItems_a[itemId].status = ARRITEM_INACTIVE;
  _sortItems();
  updateArrangementStartPositions();
  return 0; 
}

void Arrangement::updateArrangementStartPositions()
{
  for (uint8_t itemId = 0; itemId < NR_ARRITEMS; itemId++)
  {
    if (itemId == 0) arrangementItems_a[itemId].startTick = 0;
    if (arrangementItems_a[itemId].status == ARRITEM_ACTIVE && itemId > 0)
    {
      uint16_t previousArrItemStart = arrangementItems_a[itemId - 1].startTick;
      uint16_t previousArrItemLength = arrangementItems_a[itemId - 1].lengthTicks;
      arrangementItems_a[itemId].startTick = previousArrItemStart + previousArrItemLength;

//    if (itemId == 0) arrangementItems_a[itemId].startBars = 1;
//    if (arrangementItems_a[itemId].status == ARRITEM_ACTIVE && itemId > 0)
//    {
//      uint16_t previousArrItemStart = arrangementItems_a[itemId - 1].startBars;
//      uint16_t previousArrItemLength = arrangementItems_a[itemId - 1].lengthBars;
//      arrangementItems_a[itemId].startBars = previousArrItemStart + previousArrItemLength;
    }
  }
}

void Arrangement::_sortItems() { qsort(arrangementItems_a, NR_ARRITEMS, sizeof(ArrangementItem), compareArrItems); }

//void Arrangement::printItemArray(uint8_t lastIndex)
//{
//  Serial.print("Status: ");
//  for (uint8_t i = 0; i < lastIndex; i++)
//  {
//    Serial.print(arrangementItems_a[i].status);
//    Serial.print(", ");
//  }
//  Serial.println("END");
//  
//  Serial.print("Start: ");
//  for (uint8_t i = 0; i < lastIndex; i++)
//  {
//    Serial.print(arrangementItems_a[i].startBars);
//    Serial.print(", ");
//  }
//  Serial.println("END");
//
//  Serial.print("Length: ");
//  for (uint8_t i = 0; i < lastIndex; i++)
//  {
//    Serial.print(arrangementItems_a[i].lengthBars);
//    Serial.print(", ");
//  }
//  Serial.println("END");
//
//  Serial.print("Pattern: ");
//  for (uint8_t i = 0; i < lastIndex; i++)
//  {
//    Serial.print(arrangementItems_a[i].patternIndex);
//    Serial.print(", ");
//  }
//  Serial.println("END");
//}

int compareArrItems(const void *s1, const void *s2)
{
    struct ArrangementItem *e1 = (struct ArrangementItem *)s1;
    struct ArrangementItem *e2 = (struct ArrangementItem *)s2;
    // Active items first in array
    if(e1->status == ARRITEM_INACTIVE && e2->status != ARRITEM_INACTIVE) return 1;
    if(e1->status != ARRITEM_INACTIVE && e2->status == ARRITEM_INACTIVE) return -1;
//    // Then sort by tick
//    if(e1->timestamp < e2->timestamp) return -1;
//    if(e1->timestamp > e2->timestamp) return 1;
    return 0;
}
