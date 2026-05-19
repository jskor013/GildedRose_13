#include <gtest/gtest.h>
#include "GildedRose.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
const int kSimulationDays = 30;
const char* const kApprovedFileName = "gilded_rose_30_days.approved.txt";
const char* const kReceivedFileName = "gilded_rose_30_days.received.txt";

std::vector<Item> createTextTestInventory() {
    return {
        Item("+5 Dexterity Vest", 10, 20),
        Item("Aged Brie", 2, 0),
        Item("Elixir of the Mongoose", 5, 7),
        Item("Sulfuras, Hand of Ragnaros", 0, 80),
        Item("Sulfuras, Hand of Ragnaros", -1, 80),
        Item("Backstage passes to a TAFKAL80ETC concert", 15, 20),
        Item("Backstage passes to a TAFKAL80ETC concert", 10, 49),
        Item("Backstage passes to a TAFKAL80ETC concert", 5, 49),
        Item("Conjured Mana Cake", 3, 6),
    };
}

std::string renderTextTestFixtureOutput() {
    std::vector<Item> items = createTextTestInventory();
    GildedRose app(items);
    std::ostringstream output;

    output << "OMGHAI!\n";
    for (int day = 0; day <= kSimulationDays; ++day) {
        output << "\n-------- day " << day << " --------\n";
        output << "name, sellIn, quality\n";
        for (const Item& item : items) {
            output << item.toString() << '\n';
        }
        app.updateQuality();
    }

    return output.str();
}

std::filesystem::path approvalDirectory() {
#ifdef GILDED_ROSE_APPROVAL_DIR
    return std::filesystem::path(GILDED_ROSE_APPROVAL_DIR);
#else
    return std::filesystem::path("test") / "approval";
#endif
}

std::filesystem::path receivedDirectory() {
#ifdef GILDED_ROSE_RECEIVED_DIR
    return std::filesystem::path(GILDED_ROSE_RECEIVED_DIR);
#else
    return std::filesystem::temp_directory_path() / "gilded_rose_approval";
#endif
}

std::string normalizeLineEndings(std::string text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            if (i + 1 < text.size() && text[i + 1] == '\n') {
                ++i;
            }
            normalized.push_back('\n');
        } else {
            normalized.push_back(text[i]);
        }
    }
    return normalized;
}

std::string readTextFile(const std::filesystem::path& filePath) {
    std::ifstream input(filePath, std::ios::binary);
    std::ostringstream contents;
    contents << input.rdbuf();
    return contents.str();
}

void writeTextFile(const std::filesystem::path& filePath, const std::string& contents) {
    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream output(filePath, std::ios::binary);
    output << contents;
}

bool shouldUpdateApprovedFile() {
    const char* value = std::getenv("GILDED_ROSE_APPROVAL_UPDATE");
    return value != nullptr && std::string(value) == "1";
}
}

class TextTestFixture : public ::testing::Test {};

TEST_F(TextTestFixture, MatchesGoldenMasterForThirtyDays) {
    const std::filesystem::path approvedFile = approvalDirectory() / kApprovedFileName;
    const std::filesystem::path receivedFile = receivedDirectory() / kReceivedFileName;
    const std::string actual = renderTextTestFixtureOutput();

    if (shouldUpdateApprovedFile()) {
        writeTextFile(approvedFile, actual);
        SUCCEED() << "Updated approved Golden Master file: " << approvedFile.string();
        return;
    }

    if (!std::filesystem::exists(approvedFile)) {
        writeTextFile(receivedFile, actual);
        FAIL() << "Approved Golden Master file is missing: " << approvedFile.string()
               << "\nReceived output was written to: " << receivedFile.string()
               << "\nRun with GILDED_ROSE_APPROVAL_UPDATE=1 to create the approved file.";
    }

    const std::string expected = normalizeLineEndings(readTextFile(approvedFile));
    if (expected != actual) {
        writeTextFile(receivedFile, actual);
    } else if (std::filesystem::exists(receivedFile)) {
        std::filesystem::remove(receivedFile);
    }

    EXPECT_EQ(expected, actual)
        << "Golden Master output changed.\nApproved: " << approvedFile.string()
        << "\nReceived: " << receivedFile.string()
        << "\nIf the behavior change is intentional, rerun with GILDED_ROSE_APPROVAL_UPDATE=1.";
}
