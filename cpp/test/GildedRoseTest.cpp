#include <gtest/gtest.h>
#include "GildedRose.h"

class GildedRoseTest : public ::testing::Test {
protected:
    static Item updateOne(const std::string& name, int sellIn, int quality) {
        std::vector<Item> items = { Item(name, sellIn, quality) };
        GildedRose app(items);

        app.updateQuality();

        return app.items[0];
    }
};

TEST_F(GildedRoseTest, NormalItemDecreasesQualityAndSellInByOneBeforeSellDate) {
    // Given
    const Item item = updateOne("Elixir of the Mongoose", 10, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(9, actualSellIn);
    EXPECT_EQ(19, actualQuality);
}

TEST_F(GildedRoseTest, NormalItemDegradesTwiceAsFastWhenSellInIsZero) {
    // Given
    const Item item = updateOne("Elixir of the Mongoose", 0, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-1, actualSellIn);
    EXPECT_EQ(18, actualQuality);
}

TEST_F(GildedRoseTest, NormalItemDegradesTwiceAsFastWhenSellInIsNegative) {
    // Given
    const Item item = updateOne("Elixir of the Mongoose", -1, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-2, actualSellIn);
    EXPECT_EQ(18, actualQuality);
}

TEST_F(GildedRoseTest, NormalItemQualityDoesNotDropBelowZero) {
    // Given
    const Item item = updateOne("Elixir of the Mongoose", 5, 0);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(0, actualQuality);
}

TEST_F(GildedRoseTest, NormalItemAtQualityFiftyStillDecreasesNormally) {
    // Given
    const Item item = updateOne("Elixir of the Mongoose", 5, 50);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(49, actualQuality);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesQualityAndDecreasesSellInBeforeSellDate) {
    // Given
    const Item item = updateOne("Aged Brie", 10, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(9, actualSellIn);
    EXPECT_EQ(21, actualQuality);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesQualityTwiceWhenSellInIsZero) {
    // Given
    const Item item = updateOne("Aged Brie", 0, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-1, actualSellIn);
    EXPECT_EQ(22, actualQuality);
}

TEST_F(GildedRoseTest, AgedBrieIncreasesQualityTwiceWhenSellInIsNegative) {
    // Given
    const Item item = updateOne("Aged Brie", -1, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-2, actualSellIn);
    EXPECT_EQ(22, actualQuality);
}

TEST_F(GildedRoseTest, AgedBrieQualityDoesNotExceedFifty) {
    // Given
    const Item item = updateOne("Aged Brie", 5, 50);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(50, actualQuality);
}

TEST_F(GildedRoseTest, AgedBrieAtQualityZeroIncreasesToOne) {
    // Given
    const Item item = updateOne("Aged Brie", 5, 0);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(1, actualQuality);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesQualityByOneWithMoreThanTenDaysLeft) {
    // Given
    const Item item = updateOne("Backstage passes to a TAFKAL80ETC concert", 11, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(10, actualSellIn);
    EXPECT_EQ(21, actualQuality);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesQualityByTwoWithTenDaysLeft) {
    // Given
    const Item item = updateOne("Backstage passes to a TAFKAL80ETC concert", 10, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(9, actualSellIn);
    EXPECT_EQ(22, actualQuality);
}

TEST_F(GildedRoseTest, BackstagePassIncreasesQualityByThreeWithFiveDaysLeft) {
    // Given
    const Item item = updateOne("Backstage passes to a TAFKAL80ETC concert", 5, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(23, actualQuality);
}

TEST_F(GildedRoseTest, BackstagePassQualityDropsToZeroWhenSellInIsZero) {
    // Given
    const Item item = updateOne("Backstage passes to a TAFKAL80ETC concert", 0, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-1, actualSellIn);
    EXPECT_EQ(0, actualQuality);
}

TEST_F(GildedRoseTest, BackstagePassQualityDoesNotExceedFifty) {
    // Given
    const Item item = updateOne("Backstage passes to a TAFKAL80ETC concert", 5, 50);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(50, actualQuality);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeBeforeSellDate) {
    // Given
    const Item item = updateOne("Sulfuras, Hand of Ragnaros", 10, 80);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(10, actualSellIn);
    EXPECT_EQ(80, actualQuality);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeWhenSellInIsZero) {
    // Given
    const Item item = updateOne("Sulfuras, Hand of Ragnaros", 0, 80);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(0, actualSellIn);
    EXPECT_EQ(80, actualQuality);
}

TEST_F(GildedRoseTest, SulfurasDoesNotChangeWhenSellInIsNegative) {
    // Given
    const Item item = updateOne("Sulfuras, Hand of Ragnaros", -1, 80);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-1, actualSellIn);
    EXPECT_EQ(80, actualQuality);
}

TEST_F(GildedRoseTest, SulfurasKeepsQualityAboveNormalLimit) {
    // Given
    const Item item = updateOne("Sulfuras, Hand of Ragnaros", 5, 80);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(5, actualSellIn);
    EXPECT_EQ(80, actualQuality);
}

TEST_F(GildedRoseTest, SulfurasQualityZeroStillDoesNotChange) {
    // Given
    const Item item = updateOne("Sulfuras, Hand of Ragnaros", 5, 0);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(5, actualSellIn);
    EXPECT_EQ(0, actualQuality);
}

TEST_F(GildedRoseTest, ConjuredItemDegradesTwiceAsFastAsNormalBeforeSellDate) {
    // Given
    const Item item = updateOne("Conjured Mana Cake", 10, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(9, actualSellIn);
    EXPECT_EQ(18, actualQuality);
}

TEST_F(GildedRoseTest, ConjuredItemDegradesByFourWhenSellInIsZero) {
    // Given
    const Item item = updateOne("Conjured Mana Cake", 0, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-1, actualSellIn);
    EXPECT_EQ(16, actualQuality);
}

TEST_F(GildedRoseTest, ConjuredItemDegradesByFourWhenSellInIsNegative) {
    // Given
    const Item item = updateOne("Conjured Mana Cake", -1, 20);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(-2, actualSellIn);
    EXPECT_EQ(16, actualQuality);
}

TEST_F(GildedRoseTest, ConjuredItemQualityDoesNotDropBelowZero) {
    // Given
    const Item item = updateOne("Conjured Mana Cake", 5, 0);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(0, actualQuality);
}

TEST_F(GildedRoseTest, ConjuredItemAtQualityFiftyDegradesByTwo) {
    // Given
    const Item item = updateOne("Conjured Mana Cake", 5, 50);

    // When
    const int actualSellIn = item.sellIn;
    const int actualQuality = item.quality;

    // Then
    EXPECT_EQ(4, actualSellIn);
    EXPECT_EQ(48, actualQuality);
}
