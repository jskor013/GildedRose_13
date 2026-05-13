#include "GildedRose.h"
#include "GildedRoseConstants.h"

GildedRose::GildedRose(std::vector<Item>& items) : items(items) {}

void GildedRose::updateQuality() {
    using namespace GildedRoseConstants;
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].name != AGED_BRIE
                && items[i].name != BACKSTAGE_PASS) {
            if (items[i].quality > MIN_QUALITY) {
                if (items[i].name != SULFURAS) {
                    items[i].quality = items[i].quality - 1;
                }
            }
        } else {
            if (items[i].quality < 50) {
                items[i].quality = items[i].quality + 1;

                if (items[i].name == BACKSTAGE_PASS) {
                    if (items[i].sellIn < 11) {
                        if (items[i].quality < MAX_QUALITY) {
                            items[i].quality = items[i].quality + 1;
                        }
                    }

                    if (items[i].sellIn < 6) {
                        if (items[i].quality < MAX_QUALITY) {
                            items[i].quality = items[i].quality + 1;
                        }
                    }
                }
            }
        }

        if (items[i].name != SULFURAS) {
            items[i].sellIn = items[i].sellIn - 1;
        }

        if (items[i].sellIn < 0) {
            if (items[i].name != AGED_BRIE) {
                if (items[i].name != BACKSTAGE_PASS) {
                    if (items[i].quality > MIN_QUALITY) {
                        if (items[i].name != SULFURAS) {
                            items[i].quality = items[i].quality - 1;
                        }
                    }
                } else {
                    items[i].quality = items[i].quality - items[i].quality;
                }
            } else {
                if (items[i].quality < MAX_QUALITY) {
                    items[i].quality = items[i].quality + 1;
                }
            }
        }
    }
}
