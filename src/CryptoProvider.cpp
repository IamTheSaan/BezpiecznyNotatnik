#include <CryptoProvider.hpp>
#include <string>
#include <random>

std::string CryptoProvider::encrypt(const std::string data) {
    std::string result = data;

    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ serverMasterKey[i % serverMasterKey.size()];
    }
    return result;
}

std::string CryptoProvider::decrypt(const std::string data) {
    return encrypt(data);
}

std::string CryptoProvider::generateId() {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, CHARACTERS.size() - 1);

    std::string id = "";
    for (int i = 0; i < 10; i++) {
        id += CHARACTERS[(dist(rng))];
    }
    return id;
}