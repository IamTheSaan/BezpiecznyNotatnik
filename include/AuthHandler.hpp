#pragma once
#include <string>
#include <map>

// Integracja jwt-cpp z nlohmann/json (mam nadzieję, że działa, nie testowane)
#define JWT_DISABLE_PICOJSON
#include "json.hpp"
#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/nlohmann-json/traits.h"

using jwt_traits = jwt::traits::nlohmann_json;

struct UserCredentials {
    std::string passwordHash;
    std::string salt;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserCredentials, passwordHash, salt)

class AuthHandler {
private:
    std::map<std::string, UserCredentials> nameToCredentials; // username -> hasło i sól
    std::string dbPath;
    const std::string SECRET_KEY = "SekretnyKlucz123";
    std::string hash(const std::string& pass, const std::string& salt);
    void saveDb();
    void loadDb();
public:
    AuthHandler(const std::string& path);
    bool registerUser(const std::string& username, const std::string& password);
    // register jest zarezerowwane :(, zwraca true jeśli zarejestrowano, false jeśli użytkownik o podanej nazwie już istneje
    std::string login(const std::string& username, const std::string& password); // zwraca token lub ""
    std::string authenticate(const std::string& token); // zwraca username lub ""
};