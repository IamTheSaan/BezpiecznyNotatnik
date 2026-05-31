#include <gtest/gtest.h>
#include "AuthHandler.hpp"
#include <cstdio>
#include <fstream>

class AuthHandlerTest : public ::testing::Test {
protected:
    const std::string testDbPath = "test_database.json";
    AuthHandler* authHandler;

    void SetUp() override {
        std::remove(testDbPath.c_str());
        authHandler = new AuthHandler(testDbPath);
    }

    void TearDown() override {
        delete authHandler;
        std::remove(testDbPath.c_str());
    }
};

// 1. Test udanej rejestracji
TEST_F(AuthHandlerTest, RegisterNewUser_ReturnsTrue) {
    bool result = authHandler->registerUser("admin", "trudneHaslo123");
    EXPECT_TRUE(result);
}

// 2. Test rejestracji duplikatu użytkownika
TEST_F(AuthHandlerTest, RegisterDuplicateUser_ReturnsFalse) {
    authHandler->registerUser("admin", "trudneHaslo123");
    bool result = authHandler->registerUser("admin", "inneHaslo");
    EXPECT_FALSE(result);
}

// 3. Test udanego logowania
TEST_F(AuthHandlerTest, LoginValidCredentials_ReturnsToken) {
    authHandler->registerUser("admin", "mojeHaslo");
    
    std::string token = authHandler->login("admin", "mojeHaslo");
    
    // Token nie powinien być pusty
    EXPECT_NE(token, "");
}

// 4. Test logowania ze złym hasłem
TEST_F(AuthHandlerTest, LoginInvalidPassword_ReturnsEmptyString) {
    authHandler->registerUser("admin", "mojeHaslo");
    
    std::string token = authHandler->login("admin", "zleHaslo");
    
    EXPECT_EQ(token, "");
}

// 5. Test logowania na nieistniejące konto
TEST_F(AuthHandlerTest, LoginNonExistentUser_ReturnsEmptyString) {
    std::string token = authHandler->login("admin", "haslo");
    
    EXPECT_EQ(token, "");
}

// 6. Test autentykacji poprawnego tokenu (JWT)
TEST_F(AuthHandlerTest, AuthenticateValidToken_ReturnsUsername) {
    authHandler->registerUser("admin", "mojeHaslo");
    std::string token = authHandler->login("admin", "mojeHaslo");
    
    std::string username = authHandler->authenticate(token);
    
    EXPECT_EQ(username, "admin");
}

// 7. Test autentykacji fałszywym/pustym tokenem
TEST_F(AuthHandlerTest, AuthenticateInvalidToken_ReturnsEmptyString) {
    std::string username1 = authHandler->authenticate("");
    std::string username2 = authHandler->authenticate("to.nie.jest.prawdziwy.token.jwt");
    
    EXPECT_EQ(username1, "");
    EXPECT_EQ(username2, "");
}

// 8. Test mechanizmu zapisu i odczytu bazy
TEST_F(AuthHandlerTest, DatabasePersistence_LoadsExistingUsers) {
    authHandler->registerUser("admin", "haslo123");
    
    delete authHandler;
    authHandler = new AuthHandler(testDbPath);
    
    std::string token = authHandler->login("admin", "haslo123");
    
    EXPECT_NE(token, "");
}