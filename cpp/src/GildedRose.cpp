#include "GildedRose.h"
#include <string>

GildedRose::GildedRose(std::vector<Item>& items) : items(items) {}

namespace {
const std::string kAgedBrie = "Aged Brie";
const std::string kBackstagePass = "Backstage passes to a TAFKAL80ETC concert";
const std::string kSulfuras = "Sulfuras, Hand of Ragnaros";
const std::string kConjured = "Conjured";

bool isConjured(const Item& item) {
    return item.name.find(kConjured) != std::string::npos;
}

void increaseQuality(Item& item) {
    if (item.quality < 50) {
        item.quality = item.quality + 1;
    }
}

void decreaseQuality(Item& item) {
    if (item.quality > 0) {
        item.quality = item.quality - 1;
    }
}
}

void GildedRose::updateQuality() {
    for (size_t i = 0; i < items.size(); i++) {
        Item& item = items[i];

        if (item.name == kSulfuras) {
            continue;
        }

        if (item.name == kAgedBrie) {
            increaseQuality(item);
        } else if (item.name == kBackstagePass) {
            increaseQuality(item);
            if (item.sellIn < 11) {
                increaseQuality(item);
            }
            if (item.sellIn < 6) {
                increaseQuality(item);
            }
        } else if (isConjured(item)) {
            decreaseQuality(item);
            decreaseQuality(item);
        } else {
            decreaseQuality(item);
        }

        item.sellIn = item.sellIn - 1;

        if (item.sellIn < 0) {
            if (item.name == kAgedBrie) {
                increaseQuality(item);
            } else if (item.name == kBackstagePass) {
                item.quality = 0;
            } else if (isConjured(item)) {
                decreaseQuality(item);
                decreaseQuality(item);
            } else {
                decreaseQuality(item);
            }
        }
    }
}
