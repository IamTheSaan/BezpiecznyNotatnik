#pragma once
#include <string>

class CryptoProvider {
public:
    CryptoProvider() {}

    // Tymczasowo do testów
    std::string encrypt(std::string data) { return "ZASZYFROWANE_" + data; }
    std::string decrypt(std::string data) { return data; }
    std::string generateId() { return "IdABC"; }
};