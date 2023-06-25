#include "arrangement.h"

Arrangement::Arrangement()
{
  arrangementItems_a[0].status = ARRITEM_ACTIVE;
  arrangementTick = 0;
  for (uint8_t itemId = 0; itemId < NR_ARRITEMS; itemId++) memset(arrangementItems_a[itemId].muteArray, 0, NR_TRACKS);
}

uint8_t Arrangement::tick()
{
  arrangementTick++;
  return getCurrentArrItem();
}

uint8_t Arrangement::getCurrentArrItem()
{
  for (uint8_t itemId = NR_ARRITEMS; itemId > 0; itemId--)
  {
    if ( (arrangementItems_a[itemId].status == ARRITEM_ACTIVE) && (arrangementItems_a[itemId].startTick <= arrangementTick) ) return itemId;
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

void Arrangement::clear()
{
 for (uint8_t itemId = 1; itemId < NR_ARRITEMS; itemId++)
  {
    arrangementItems_a[itemId].status = ARRITEM_INACTIVE;
  }
  arrangementItems_a[0].startTick = 0;
  arrangementItems_a[0].lengthTicks = 4 * 4 * RESOLUTION;
  arrangementItems_a[0].patternIndex = 0;
  arrangementItems_a[0].status = ARRITEM_ACTIVE;
  memset(arrangementItems_a[0].muteArray, 0, NR_TRACKS);
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
    }
  }
}

void Arrangement::moveItemUp()
{

}

void Arrangement::moveItemDown()
{
  
}

void Arrangement::_sortItems() { qsort(arrangementItems_a, NR_ARRITEMS, sizeof(ArrangementItem), compareArrItems); }

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
