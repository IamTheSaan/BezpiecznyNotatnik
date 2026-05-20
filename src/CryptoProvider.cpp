#include <CryptoProvider.hpp>
#include <random>
#include <stdexcept>
#include <cstring>
#include <algorithm>

// Resize klucza
CryptoProvider::CryptoProvider(std::string key) {
    masterKey.resize(8, 0);
    for (size_t i = 0; i < 32 && i < key.length(); ++i) {
        masterKey[i / 4] |= (static_cast<uint32_t>(static_cast<uint8_t>(key[i])) << (8 * (i % 4)));
    }
}

// Rotacja bitowa w lewo
uint32_t CryptoProvider::rotl(uint32_t value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}

// Operacja ARX
void CryptoProvider::quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = rotl(d, 16);
    c += d; b ^= c; b = rotl(b, 12);
    a += b; d ^= a; d = rotl(d, 8);
    c += d; b ^= c; b = rotl(b, 7);
}

// Główna funkcja generująca 64-bajtowy blok
std::vector<uint8_t> CryptoProvider::chacha20Block(uint32_t counter, const std::vector<uint8_t>& nonce) {
    uint32_t state[16];
    
    // Stałe "expand 32-byte k"
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;
    
    // Klucz 256-bitowy
    for (int i = 0; i < 8; ++i) state[4 + i] = masterKey[i];
    
    // Licznik bloków
    state[12] = counter;
    
    // Nonce 96-bitowy
    for (int i = 0; i < 3; ++i) {
        state[13 + i] = static_cast<uint32_t>(nonce[i*4]) |
                        (static_cast<uint32_t>(nonce[i*4 + 1]) << 8) |
                        (static_cast<uint32_t>(nonce[i*4 + 2]) << 16) |
                        (static_cast<uint32_t>(nonce[i*4 + 3]) << 24);
    }

    // Kopia zapasowa stanu do późniejszego dodania
    uint32_t workingState[16];
    std::memcpy(workingState, state, 16 * sizeof(uint32_t));

    // 20 rund mieszania (10 iteracji)
    for (int i = 0; i < 10; ++i) {
        // Rundy kolumnowe
        quarterRound(workingState[0], workingState[4], workingState[8], workingState[12]);
        quarterRound(workingState[1], workingState[5], workingState[9], workingState[13]);
        quarterRound(workingState[2], workingState[6], workingState[10], workingState[14]);
        quarterRound(workingState[3], workingState[7], workingState[11], workingState[15]);
        // Rundy przekątniowe
        quarterRound(workingState[0], workingState[5], workingState[10], workingState[15]);
        quarterRound(workingState[1], workingState[6], workingState[11], workingState[12]);
        quarterRound(workingState[2], workingState[7], workingState[8], workingState[13]);
        quarterRound(workingState[3], workingState[4], workingState[9], workingState[14]);
    }

    // Dodanie końcowe wymieszanego stanu do stanu początkowego
    for (int i = 0; i < 16; ++i) {
        state[i] += workingState[i];
    }

    // Ekstrakcja 64 bajtów szumu w formacie Little-Endian
    std::vector<uint8_t> block(64);
    for (int i = 0; i < 16; ++i) {
        block[i*4 + 0] = (state[i] >> 0) & 0xFF;
        block[i*4 + 1] = (state[i] >> 8) & 0xFF;
        block[i*4 + 2] = (state[i] >> 16) & 0xFF;
        block[i*4 + 3] = (state[i] >> 24) & 0xFF;
    }
    return block;
}

std::string CryptoProvider::process(const std::string& data, const std::vector<uint8_t>& nonce) {
    std::string result = data;
    uint32_t counter = 0;

    // Przechodzimy przez wiadomość partiami po 64 bajty
    for (size_t i = 0; i < data.size(); i += 64) {
        std::vector<uint8_t> keyStream = chacha20Block(counter++, nonce);
        
        size_t chunk_size = std::min<size_t>(64, data.size() - i);
        for (size_t j = 0; j < chunk_size; ++j) {
            result[i + j] ^= keyStream[j];
        }
    }
    return result;
}

std::string CryptoProvider::encrypt(const std::string& data) {
    std::vector<uint8_t> nonce(12);
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < 12; ++i) {
        nonce[i] = dist(rng);
    }

    std::string cipherText = process(data, nonce);
    
    std::string result(nonce.begin(), nonce.end());
    result += cipherText;
    return result;
}

std::string CryptoProvider::decrypt(const std::string& data) {

    if (data.size() < 12) {
        throw std::runtime_error("Ciphertext is too short (missing nonce)!");
    }

    std::vector<uint8_t> nonce(data.begin(), data.begin() + 12);
    
    std::string cipherText = data.substr(12);

    return process(cipherText, nonce);
}

std::string CryptoProvider::generateId() {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, CHARACTERS.size() - 1);

    std::string id = "";
    for (int i = 0; i < 16; i++) {
        id += CHARACTERS[(dist(rng))];
    }
    return id;
}