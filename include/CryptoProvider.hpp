#pragma once
#include <string>
#include <vector>
#include <cstdint>

class CryptoProvider {
private:
    std::vector<uint32_t> masterKey;
    static uint32_t rotl(uint32_t value, int shift);
    static void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
    std::vector<uint8_t> chacha20Block(uint32_t counter, const std::vector<uint8_t>& nonce);
    std::string process(const std::string& data, const std::vector<uint8_t>& nonce);

public:
    CryptoProvider(std::string key);

    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);
    std::string generateId();
};