#include <gtest/gtest.h>
#include <memory>
#include <CryptoProvider.hpp>

TEST(CryptoProviderTest, EncryptionLoop) {
    Context crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR"));

    std::string input = "Litwo! Ojczyzno moja! ty jesteś jak zdrowie.Ile cię trzeba cenić, ten tylko się dowie,Kto cię stracił. Dziś piękność twą w całej ozdobieWidzę i opisuję, bo tęsknię po tobie.";
    std::string encrypted = crypto.encrypt(input);
    std::string decrypted = crypto.decrypt(encrypted);

    EXPECT_EQ(decrypted, input);
}

TEST(CryptoProviderTest, EncryptionChangesData) {
    Context crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR"));

    std::string input = "Litwo! Ojczyzno moja! ty jesteś jak zdrowie.Ile cię trzeba cenić, ten tylko się dowie,Kto cię stracił. Dziś piękność twą w całej ozdobieWidzę i opisuję, bo tęsknię po tobie.";
    std::string encrypted = crypto.encrypt(input);

    EXPECT_NE(encrypted, input);
}

TEST(CryptoProviderTest, GenerateIdLength) {
    Context crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR"));

    std::string id = crypto.generateId();

    EXPECT_EQ(id.length(), 10);
}

TEST(CryptoProviderTest, GenerateIdCharacters) {
    Context crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR"));

    std::string id = crypto.generateId();
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (char c : id) {
        EXPECT_NE(chars.find(c), -1);
    }
}

TEST(CryptoProviderTest, GenerateIdDifferentValues) {
    Context crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR"));

    std::string id1 = crypto.generateId();
    std::string id2 = crypto.generateId();

    EXPECT_NE(id1, id2);
}