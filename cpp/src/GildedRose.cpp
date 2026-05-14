#include "GildedRose.h"
#include "GildedRoseConstants.h"

GildedRose::GildedRose(std::vector<Item>& items) : items(items) {}

void GildedRose::updateQuality() {
    using namespace GildedRoseConstants;
    for (size_t i = 0; i < items.size(); i++) {
        Item& item = items[i];
        if (item.name == CONJURED) {
            // 2배 감소, sellIn 지나면 4배
            int degradeBy = (item.sellIn > 0) ? 2 : 4;
            item.quality = std::max(MIN_QUALITY,
                                    item.quality - degradeBy);

            item.sellIn--;
            continue;
        }
        if (item.name != AGED_BRIE
                && item.name != BACKSTAGE_PASS) {
            if (item.quality > MIN_QUALITY) {
                if (item.name != SULFURAS) {
                    item.quality = item.quality - 1;
                }
            }
        } else {
            if (item.quality < MAX_QUALITY) {
                item.quality = item.quality + 1;

                if (item.name == BACKSTAGE_PASS) {
                    if (item.sellIn < BACKSTAGE_PASS_X2_BOUNDARY) {
                        if (item.quality < MAX_QUALITY) {
                            item.quality = item.quality + 1;
                        }
                    }

                    if (item.sellIn < BACKSTAGE_PASS_X3_BOUNDARY) {
                        if (item.quality < MAX_QUALITY) {
                            item.quality = item.quality + 1;
                        }
                    }
                }
            }
        }

        if (item.name != SULFURAS) {
            item.sellIn = item.sellIn - 1;
        }

        if (item.sellIn < 0) {
            if (item.name != AGED_BRIE) {
                if (item.name != BACKSTAGE_PASS) {
                    if (item.quality > MIN_QUALITY) {
                        if (item.name != SULFURAS) {
                            item.quality = item.quality - 1;
                        }
                    }
                } else {
                    item.quality = item.quality - item.quality;
                }
            } else {
                if (item.quality < MAX_QUALITY) {
                    item.quality = item.quality + 1;
                }
            }
        }
    }
}
