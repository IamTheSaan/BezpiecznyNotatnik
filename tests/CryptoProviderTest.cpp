#include <gtest/gtest.h>
#include <CryptoProvider.hpp>

TEST(CryptoProviderTest, EncryptionLoop) {
    CryptoProvider crypto;

    std::string input = "Litwo! Ojczyzno moja! ty jesteś jak zdrowie.Ile cię trzeba cenić, ten tylko się dowie,Kto cię stracił. Dziś piękność twą w całej ozdobieWidzę i opisuję, bo tęsknię po tobie.";
    std::string encrypted = crypto.encrypt(input);
    std::string decrypted = crypto.decrypt(encrypted);

    EXPECT_EQ(decrypted, input);
}

TEST(CryptoProviderTest, EncryptionNotRandom) {
    CryptoProvider crypto;

    std::string input = "Litwo! Ojczyzno moja! ty jesteś jak zdrowie.Ile cię trzeba cenić, ten tylko się dowie,Kto cię stracił. Dziś piękność twą w całej ozdobieWidzę i opisuję, bo tęsknię po tobie.";
    std::string encrypted1 = crypto.encrypt(input);
    std::string encrypted2 = crypto.encrypt(input);

    EXPECT_EQ(encrypted1, encrypted2);
}

TEST(CryptoProviderTest, EncryptionChangesData) {
    CryptoProvider crypto;

    std::string input = "Litwo! Ojczyzno moja! ty jesteś jak zdrowie.Ile cię trzeba cenić, ten tylko się dowie,Kto cię stracił. Dziś piękność twą w całej ozdobieWidzę i opisuję, bo tęsknię po tobie.";
    std::string encrypted = crypto.encrypt(input);

    EXPECT_NE(encrypted, input);
}

TEST(CryptoProviderTest, GenerateIdLength) {
    CryptoProvider crypto;

    std::string id = crypto.generateId();

    EXPECT_EQ(id.length(), 10);
}

TEST(CryptoProviderTest, GenerateIdCharacters) {
    CryptoProvider crypto;

    std::string id = crypto.generateId();
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (char c : id) {
        EXPECT_NE(chars.find(c), -1);
    }
}

TEST(CryptoProviderTest, GenerateIdDifferentValues) {
    CryptoProvider crypto;

    std::string id1 = crypto.generateId();
    std::string id2 = crypto.generateId();

    EXPECT_NE(id1, id2);
}