#define APPROVALS_GOOGLETEST
#include "ApprovalTests.hpp"
#include "GildedRose.h"
#include <sstream>

std::string simulate30Days(std::vector<Item> items) {
    GildedRose gr(items);
    std::ostringstream oss;

    for (int day = 0; day <= 30; ++day) {
        oss << "-------- day " << day << " --------\n";

        for (const auto& item : gr.items) {
            oss << item.name << ", " << item.sellIn
                << ", " << item.quality << "\n";
        }

        if (day < 30) gr.updateQuality();
    }

    return oss.str();
}

TEST(GildedRoseApproval, ThirtyDaysSimulation) {
    std::vector<Item> items = {
        Item("+5 Dexterity Vest", 10, 20),
        Item("Aged Brie", 2, 0),
        Item("Elixir of the Mongoose", 5, 7),
        Item("Sulfuras, Hand of Ragnaros", 0, 80),
        Item("Sulfuras, Hand of Ragnaros", -1, 80),
        Item("Backstage passes to a TAFKAL80ETC concert", 15, 20),
        Item("Backstage passes to a TAFKAL80ETC concert", 10, 49),
        Item("Backstage passes to a TAFKAL80ETC concert", 5, 49),
        Item("Conjured Mana Cake", 3, 6),  // 신규!
    };

    // approved.txt와 자동 비교 - 한 줄로 전체 동작 고정!
    ApprovalTests::Approvals::verify(simulate30Days(items));
}


