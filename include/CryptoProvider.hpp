#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
class CryptoProvider {
    public:
    virtual ~CryptoProvider() = default;
    virtual std::string encrypt(const std::string& data) const = 0;
    virtual std::string decrypt(const std::string& data) const = 0;
};

class Context {
    private:
    std::unique_ptr<CryptoProvider> strategy_;
    public:
    explicit Context(std::unique_ptr<CryptoProvider> strategy = nullptr);
    void setStrategy(std::unique_ptr<CryptoProvider> newStrategy);
    std::string encrypt (const std::string& data) const;
    std::string decrypt (const std::string& data) const;
    std::string generateId();
};

class ChaCha20 : public CryptoProvider{
    private:
    std::vector<uint32_t> masterKey;
    static uint32_t rotl(uint32_t value, int shift);
    static void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d);
    std::vector<uint8_t> chacha20Block(uint32_t counter, const std::vector<uint8_t>& nonce) const;
    std::string process(const std::string& data, const std::vector<uint8_t>& nonce) const;
    public:
    ChaCha20(std::string key);
    std::string encrypt(const std::string& data) const override;
    std::string decrypt(const std::string& data) const override;
};

class XOR : public CryptoProvider{
    private:
    std::string key;
    public:
    XOR(std::string key);
    std::string encrypt(const std::string& data) const override;
    std::string decrypt(const std::string& data) const override;
};