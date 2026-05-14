#include "GildedRose.h"
#include "GildedRoseConstants.h"

using namespace GildedRoseConstants;
bool isNormalItem(Item item)
{
    return (item.name != AGED_BRIE) && (item.name != BACKSTAGE_PASS) && (item.name != CONJURED) && (item.name != SULFURAS);
}

GildedRose::GildedRose(std::vector<Item>& items) : items(items) {}

void GildedRose::updateQuality() {
    
    for (size_t i = 0; i < items.size(); i++) 
    {
        Item& item = items[i];
        if (isNormalItem(item)) 
        {
            if(item.sellIn == 0) item.quality -= 2;
            else if(item.sellIn > 0) item.quality -= 1;
        } 
        else if (item.name == CONJURED)
        {
            // 2배 감소, sellIn 지나면 4배
            int degradeBy = (item.sellIn > 0) ? 2 : 4;
            item.quality = std::max(MIN_QUALITY, item.quality - degradeBy);            
        }
        else if(item.name == BACKSTAGE_PASS)
        {    
            if(item.sellIn <= 0) item.quality = 0;          
            else if(item.sellIn<BACKSTAGE_PASS_X3_BOUNDARY) item.quality += 3;
            else if(item.sellIn<BACKSTAGE_PASS_X2_BOUNDARY) item.quality += 2;            
            else item.quality += 1;                     
        }
        else if(item.name == SULFURAS)
        {
            item.quality = 80;
        }
        else if(item.name == AGED_BRIE)
        {
            if(item.sellIn == 0) item.quality += 2;
            else if(item.sellIn > 0) item.quality += 1;
        }

        if(item.name == SULFURAS)
        {
            item.sellIn;            
        }
        else
        {
            item.sellIn--; 
        }

        if(item.sellIn <= 0) item.sellIn = 0;
        if(item.name != SULFURAS)
        {
            if(item.quality > MAX_QUALITY) item.quality = MAX_QUALITY;
            if(item.quality < MIN_QUALITY) item.quality = MIN_QUALITY;
        }

    }


        
        

        
}
