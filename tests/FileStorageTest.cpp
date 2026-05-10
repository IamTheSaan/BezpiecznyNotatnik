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
    EXPECT_TRUE(fs::exists(testPath + "/Recipient/IdABC.bin"));
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

TEST_F(FileStorageTest, Delete) {
    NoteData data = {"IdABC", "A", 0, false, "B"};
    storage->saveFile("UserC", data);
    std::string path = testPath + "/UserC/IdABC.bin";
    
    ASSERT_TRUE(fs::exists(path));
    EXPECT_TRUE(storage->deleteFile(path));
    EXPECT_FALSE(fs::exists(path));
}
