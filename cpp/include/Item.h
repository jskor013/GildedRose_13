#pragma once
#include <string>

class Item {
public:
    std::string name;
    int sellIn;
    int quality;

    Item(std::string name, int sellIn, int quality)
        : name(std::move(name)), sellIn(sellIn), quality(quality) {}

    std::string toString() const {
        return name + ", " + std::to_string(sellIn) + ", " + std::to_string(quality);
    }
};
