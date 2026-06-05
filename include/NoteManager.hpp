#pragma once

#include <string>
#include <vector>
#include <memory>
#include "NoteData.hpp"
#include "FileStorage.hpp"
#include "CryptoProvider.hpp"

class NoteManager {
private:
    Context crypto;  
    FileStorage storage;
public:
    NoteManager(const std::string& key) : crypto(std::make_unique<ChaCha20>(key)) {}
    bool sendNote(std::string from, std::string to, std::string text, int secondsToLive, bool burnOnRead);
    bool deleteNote(std::string user, std::string id);
    std::vector<NoteData> getInbox(std::string username);
    FileStorage& getStorage() { return storage; }
};