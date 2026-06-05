#include "AuthHandler.hpp"
#include <openssl/rand.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/evp.h>

AuthHandler::AuthHandler(const std::string& path, const std::string& secret) : dbPath(path), JWT_SECRET(secret) {
    loadDb();
}

std::string AuthHandler::hash(const std::string& pass, const std::string& salt) {
    const int iterations = 10000;
    const int  key_lenght = 32;
    unsigned char hashResult[key_lenght];
    
    if(PKCS5_PBKDF2_HMAC(pass.c_str(),
                         pass.length(),
                         reinterpret_cast<const unsigned char*>(salt.c_str()),
                         salt.length(),
                         iterations,
                         EVP_sha256(),
                         key_lenght,
                         hashResult)==0) throw std::runtime_error("Blad hashowania");
    std::stringstream ss;
    for(int i=0;i<key_lenght;i++){
        ss<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(hashResult[i]);
    }
    return ss.str();
}


std::string generateSalt() {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int salt_length=16;
    const int limit=255-255%CHARACTERS.size();
    std::string salt = "";
    salt.reserve(16);
    while(salt.length()<salt_length){
        unsigned char random_byte;
        RAND_bytes(&random_byte, 1);

        if (random_byte < limit) {
            int index = random_byte % CHARACTERS.size();
            salt += CHARACTERS[index];
        }
    }
    return salt;
}

bool AuthHandler::registerUser(const std::string& username, const std::string& password) {
    if (nameToCredentials.find(username)!=nameToCredentials.end()) {
        std::cout << "Uzytkownik " << username << " juz istnieje.\n";
        return false; 
    }

    std::string salt = generateSalt();
    std::string hashedPassword = hash(password, salt);


    UserCredentials creds = {hashedPassword, salt};
    nameToCredentials[username] = creds;

    saveDb();
    
    std::cout << "Pomyslnie zarejestrowano uzytkownika: " << username << "\n";
    return true;
}

std::string AuthHandler::login(const std::string& username, const std::string& password) {
    auto it = nameToCredentials.find(username);
    if (it == nameToCredentials.end()) {
        return "";
    }

    UserCredentials creds = it->second;
    std::string attemptedHash = hash(password, creds.salt);

    if (attemptedHash != creds.passwordHash) {
        return "";
    }

    auto token = jwt::create<jwt_traits>()
        .set_issuer("auth_server")
        .set_type("JWS")
        .set_payload_claim("username", username)
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes(30)) 
        .sign(jwt::algorithm::hs256{JWT_SECRET});

    return token;
}

std::string AuthHandler::authenticate(const std::string& token) {
    if (token.empty()) return "";

    try {
        auto decoded = jwt::decode<jwt_traits>(token);
        auto verifier = jwt::verify<jwt_traits>()
            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
            .with_issuer("auth_server");
        
        verifier.verify(decoded);
        return decoded.get_payload_claim("username").as_string();
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd autentykacji: " << e.what() << "\n";
        return "";
    }
}

void AuthHandler::saveDb() {
    nlohmann::json j = nameToCredentials;
    
    std::ofstream file(dbPath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    } else {
        std::cerr << "Nie udalo sie otworzyc pliku bazy do zapisu: " << dbPath << "\n";
    }
}

void AuthHandler::loadDb() {
    std::ifstream file(dbPath);
    if (file.is_open()) {
        nlohmann::json j;
        try {
            file >> j;
            nameToCredentials = j.get<std::map<std::string, UserCredentials>>();
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Blad parsowania bazy uzytkownikow: " << e.what() << "\n";
        }
        file.close();
    }
}