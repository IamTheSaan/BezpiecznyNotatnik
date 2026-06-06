#include <gtest/gtest.h>
#include <FileStorage.hpp>
#include <filesystem>

namespace fs = std::filesystem;

class FileStorageTest : public ::testing::Test {
protected:
    std::string testPath = "test_dir";
    FileStorage* storage;
    void SetUp() override {
        storage = new FileStorage(testPath);
        if (fs::exists(testPath)) fs::remove_all(testPath);
        fs::create_directory(testPath);
    }
    void TearDown() override {
        delete storage;
        if (fs::exists(testPath)) fs::remove_all(testPath);
    }
};

TEST_F(FileStorageTest, CreateFile) {
    NoteData data = {"IdABC", "UserA", 100, false, "Hello"};
    bool result = storage->saveFile("Recipient", data);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists(testPath + "/Recipient/IdABC.enc"));
}

TEST_F(FileStorageTest, CreateDirectory) {
    NoteData data = {"IdABC", "UserA", 100, false, "Hello"};
    storage->saveFile("NewUser", data);
    
    EXPECT_TRUE(fs::is_directory(testPath + "/NewUser"));
}

TEST_F(FileStorageTest, Notes) {
    NoteData data1 = {"IdABC", "Sender", 0, false, "Text"};
    NoteData data2 = {"IdDEF", "Sender", 0, false, "Text"};
    storage->saveFile("UserB", data1);
    storage->saveFile("UserB", data2);
    
    auto notes = storage->readFiles("UserB");
    EXPECT_EQ(notes.size(), 2);
}

TEST_F(FileStorageTest, EmptyNotes) {
    auto notes = storage->readFiles("EmptyUser");
    EXPECT_TRUE(notes.empty());
}

TEST_F(FileStorageTest, CleanupExpired) {
    NoteData data = {"Id123", "UserA", 100, false, "Hello"};
    storage->saveFile("UserA", data);
    
    EXPECT_TRUE(fs::exists(testPath + "/UserA/Id123.enc"));
    
    // Odpalamy czyszczenie z czasem 150 (150 > 100 => notatka wygasła)
    storage->cleanup(150);
    
    // Plik powinien być usunięty
    EXPECT_FALSE(fs::exists(testPath + "/UserA/Id123.enc"));
}

TEST_F(FileStorageTest, CleanupKeepsActiveNotes) {
    NoteData data = {"Id456", "UserA", 200, false, "Hello"};
    storage->saveFile("UserA", data);
    
    // Odpalamy czyszczenie z czasem 150 (150 < 200 => notatka jeszcze nie wygasła)
    storage->cleanup(150);
    
    // Plik nadal powinien istnieć
    EXPECT_TRUE(fs::exists(testPath + "/UserA/Id456.enc"));
}

TEST_F(FileStorageTest, CleanupIgnorePermanent) {
    // 0 - notatka bezterminowa
    NoteData data = {"Id789", "UserA", 0, false, "Hello"};
    storage->saveFile("UserA", data);
    
    // Czyszczenie w przyszłości
    storage->cleanup(999999);
    
    // Plik z czasem 0 musi przetrwać sprzątanie
    EXPECT_TRUE(fs::exists(testPath + "/UserA/Id789.enc"));
}

TEST_F(FileStorageTest, CleanupMixed) {
    NoteData expired = {"Id1", "UserB", 50, false, "Text"};
    NoteData active = {"Id2", "UserB", 300, false, "Text"};
    NoteData permanent = {"Id3", "UserB", 0, false, "Text"};
    
    storage->saveFile("UserB", expired);
    storage->saveFile("UserB", active);
    storage->saveFile("UserB", permanent);
    
    storage->cleanup(100);
    
    EXPECT_FALSE(fs::exists(testPath + "/UserB/Id1.enc"));
    EXPECT_TRUE(fs::exists(testPath + "/UserB/Id2.enc"));
    EXPECT_TRUE(fs::exists(testPath + "/UserB/Id3.enc"));
    
    auto notes = storage->readFiles("UserB");
    EXPECT_EQ(notes.size(), 2);
}
