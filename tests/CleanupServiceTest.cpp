#include <gtest/gtest.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include "FileStorage.hpp"
#include "CleanupService.hpp"

namespace fs = std::filesystem;

class CleanupServiceTest : public ::testing::Test {
protected:
    std::string testPath = "test_cleanup_service_dir";
    FileStorage* storage;

    void SetUp() override {
        if (fs::exists(testPath)) fs::remove_all(testPath);
        fs::create_directory(testPath);
        storage = new FileStorage(testPath);
    }

    void TearDown() override {
        delete storage;
        if (fs::exists(testPath)) fs::remove_all(testPath);
    }
};

TEST_F(CleanupServiceTest, StartAndStop) {
    CleanupService service(*storage, 1);
    
    service.start();
    
    // Dajemy czas na wystartowanie wątku
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Zatrzymujemy usługę
    service.stop();
    
    SUCCEED();
}

TEST_F(CleanupServiceTest, MultipleStartsAndStops) {
    CleanupService service(*storage, 1);
    
    service.start();
    service.start();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    service.stop();
    service.stop();
    
    SUCCEED();
}

TEST_F(CleanupServiceTest, RemovesExpiredFiles) {
    NoteData expiredData = {"IdExpired", "User", 100, false, "Hello"};
    storage->saveFile("User", expiredData);
    
    NoteData permanentData = {"IdPermanent", "User", 0, false, "Hello"};
    storage->saveFile("User", permanentData);
    
    NoteData activeData = {"IdActive", "User", 10413788400, false, "Hello"}; // rok 2300 - nie uruchamiać później ;)
    storage->saveFile("User", activeData);

    EXPECT_TRUE(fs::exists(testPath + "/User/IdExpired.bin"));
    EXPECT_TRUE(fs::exists(testPath + "/User/IdPermanent.bin"));
    EXPECT_TRUE(fs::exists(testPath + "/User/IdActive.bin"));
    
    CleanupService service(*storage, 1);
    service.start();
    
    // Czekamy 1.5 sekundy aby zagwarantować uruchomienie pętli
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    
    service.stop();
    
    EXPECT_FALSE(fs::exists(testPath + "/User/IdExpired.bin"));
    EXPECT_TRUE(fs::exists(testPath + "/User/IdPermanent.bin"));
    EXPECT_TRUE(fs::exists(testPath + "/User/IdActive.bin"));
}