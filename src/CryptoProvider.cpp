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
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 999999);

    std::string id;
    id="ID_"+std::to_string(dist(rng));
    return id;
}