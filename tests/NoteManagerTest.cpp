#include <gtest/gtest.h>
#include <filesystem>
#include "NoteManager.hpp"

namespace fs = std::filesystem;

class NoteManagerTest : public ::testing::Test {
protected:
    NoteManager* manager;
    std::string key = "q7M8zHR9OylOCqwnQa5t7pYVutAYjhQLdN+U76kW5hA=";

    void SetUp() override {
        manager = new NoteManager(key);
        std::string path = manager->getStorage().getBasePath();
        if (fs::exists(path)) fs::remove_all(path);
        fs::create_directories(path);
    }

    void TearDown() override {
        std::string path = manager->getStorage().getBasePath();
        if (fs::exists(path)) fs::remove_all(path);
        delete manager;
    }
};

TEST_F(NoteManagerTest, SendNote) {    
    EXPECT_TRUE(manager->sendNote("Ania", "Tomek", "Hej!", 3600, false));
    
    // Sprawdzenie zapisu
    std::string fullPath = manager->getStorage().getBasePath() + "/Tomek";
    EXPECT_TRUE(fs::exists(fullPath));
}

TEST_F(NoteManagerTest, GetNotes) {
    manager->sendNote("Tytus", "User", "Yo!", 3600, false);
    manager->sendNote("Romek", "User", "Yo yo!", 3600, false);
    manager->sendNote("A'Tomek", "User", "Yo yo yo!", 3600, false);
    auto notes = manager->getInbox("User");
    EXPECT_EQ(notes.size(), 3);
}

TEST_F(NoteManagerTest, BurnOnRead) {
    manager->sendNote("System", "User", "Top Secret", 10, true);

    // Pierwszy odczyt
    auto notes = manager->getInbox("User");
    ASSERT_EQ(notes.size(), 1);

    // Drugi odczyt - powinno zniknąć
    notes = manager->getInbox("User");
    EXPECT_EQ(notes.size(), 0);
    std::string userDir = manager->getStorage().getBasePath() + "/User";
    EXPECT_TRUE(fs::is_empty(userDir));
}

TEST_F(NoteManagerTest, GetEmpty) {
    auto notes = manager->getInbox("User");
    EXPECT_EQ(notes.size(), 0);
}