#include <gtest/gtest.h>
#include "GildedRose.h"

// 경계값: quality는 0 미만이 되어선 안 된다
TEST(GildedRoseTest, QualityNeverNegative) {
    std::vector<Item> items = {Item("Normal Item", 5, 0)};
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_GE(items[0].quality, 0);  // quality >= 0 보장
}

// 경계값: quality는 50을 초과해선 안 된다
TEST(GildedRoseTest, AgedBrieQualityMax50) {
    std::vector<Item> items = {Item("Aged Brie", 5, 50)};
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_LE(items[0].quality, 50);  // quality <= 50 보장
}

// 경계값: sellIn 지나면 quality 2배 감소
TEST(GildedRoseTest, NormalItemDegradesTwiceAfterSellDate) {
    std::vector<Item> items = {Item("Normal Item", 0, 10)};
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_EQ(8, items[0].quality);  // 10 - 2 = 8
}

TEST(GildedRoseTest, SulfurasNoChangesAfterUpdate) {
    std::vector<Item> items = {Item("Sulfuras, Hand of Ragnaros", 50, 80)};
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_EQ(80, items[0].quality);  // 10 - 2 = 8
}

// Backstage Passes 경계값 - 파라미터화 테스트 (C++)
class BackstagePassTest
    : public ::testing::TestWithParam<
          std::tuple<int, int, int>> {};

TEST_P(BackstagePassTest, QualityUpdate) {
    auto [sellIn, initQ, expectedQ] = GetParam();
    std::vector<Item> items = {
        Item("Backstage passes to a TAFKAL80ETC concert",
             sellIn, initQ)
    };
    GildedRose gr(items);
    gr.updateQuality();    
    EXPECT_EQ(expectedQ, items[0].quality);
}

INSTANTIATE_TEST_SUITE_P(
    BackstageBoundary, BackstagePassTest,
    ::testing::Values(
        // (sellIn, initQ, expectedQ)
        std::make_tuple(15, 20, 21),  // > 10: +1
        std::make_tuple(11, 20, 21),  // 경계: sellIn=11 -> +1
        std::make_tuple(10, 20, 22),  // 경계: sellIn=10 -> +2
        std::make_tuple(6, 20, 22),   // 경계: sellIn=6 -> +2
        std::make_tuple(5, 20, 23),   // 경계: sellIn=5 -> +3
        std::make_tuple(1, 20, 23),   // 경계: sellIn=1 -> +3
        std::make_tuple(0, 20, 0),    // 경계: sellIn=0 -> quality=0
        std::make_tuple(5, 50, 50),   // quality 상한 50
        std::make_tuple(0, 50, 0)     // concert 후 -> 0
    )
);

// ① RED - 실패하는 테스트 먼저 작성
TEST(ConjuredTest, DegradesTwiceNormal) {
    std::vector<Item> items = {
        Item("Conjured Mana Cake", 10, 20)
    };
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_EQ(18, items[0].quality); // 20 - 2 = 18 ← 아직 실패!
}

TEST(ConjuredTest, DegradesTwiceAfterSellDate) {
    std::vector<Item> items = {
        Item("Conjured Mana Cake", 0, 10)
    };
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_EQ(6, items[0].quality);  // 10 - 4 = 6 ← 아직 실패!
}

TEST(ConjuredTest, QualityNeverNegative) {
    std::vector<Item> items = {
        Item("Conjured Mana Cake", 5, 1)
    };
    GildedRose gr(items);
    gr.updateQuality();
    EXPECT_EQ(0, items[0].quality);  // 1 - 2 → 0 (음수 방지)
}