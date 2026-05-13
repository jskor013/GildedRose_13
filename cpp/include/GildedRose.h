#pragma once
#include "Item.h"
#include <vector>

class GildedRose {
public:
    std::vector<Item>& items;

    GildedRose(std::vector<Item>& items);
    void updateQuality();
};
