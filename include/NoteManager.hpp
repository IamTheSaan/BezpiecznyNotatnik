#pragma once

#include <string>
#include <vector>
#include "NoteData.hpp"
#include "FileStorage.hpp"
#include "CryptoProvider.hpp"

class NoteManager {
private:
    CryptoProvider crypto;  
    FileStorage storage;
public:
    NoteManager() {}
    bool sendNote(std::string from, std::string to, std::string text, int secondsToLive, bool burnOnRead);
    std::vector<NoteData> getInbox(std::string username);
    FileStorage& getStorage() { return storage; } // do testów
};