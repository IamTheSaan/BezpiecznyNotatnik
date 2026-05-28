#pragma once
#include <string>
#include <map>

// Integracja jwt-cpp z nlohmann/json (mam nadzieję, że działa, nie testowane)
#define JWT_DISABLE_PICOJSON
#include "json.hpp"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/nlohmann-json/traits.h"
#include <iostream> // do usunięcia

using jwt_traits = jwt::traits::nlohmann_json;

struct UserCredentials {
    std::string passwordHash;
    std::string salt;
};

class AuthHandler {
private:
    std::map<std::string, UserCredentials> nameToCredentials; // username -> hasło i sól
    // map<string, string> tokenToName; Zastąpione przez JWT
    std::string dbPath;
    const std::string SECRET_KEY = "SekretnyKlucz123";

public:
    AuthHandler(const std::string& path) : dbPath(path) {}
    bool registerUser(const std::string& username, const std::string& password) {std::cout << "Zarejestrowano " << username << " " << password; return true;}
    // register jest zarezerowwane :(, zwraca true jeśli zarejestrowano, false jeśli użytkownik o podanej nazwie już istneje
    std::string login(const std::string& username, const std::string& password) {return "Token123";} // zwraca token lub ""
    std::string authenticate(const std::string& token) {return "User123";} // zwraca username lub ""
    // void logout(string token); Wylogowanie realizuje teraz frontend (usunięcie tokenu)
    void saveDb();
    void loadDb();
};