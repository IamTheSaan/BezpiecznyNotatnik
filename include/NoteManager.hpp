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
    NoteManager() : crypto(std::make_unique<ChaCha20>("+%v4g8nf6dS#@.zC->,KBJ6>YICQ&HeR")) {}
    bool sendNote(std::string from, std::string to, std::string text, int secondsToLive, bool burnOnRead);
    bool deleteNote(std::string user, std::string id);
    std::vector<NoteData> getInbox(std::string username);
    FileStorage& getStorage() { return storage; }
};