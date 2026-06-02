#include "NoteManager.hpp"
#include <chrono>

bool NoteManager::sendNote(std::string from, std::string to, std::string text, int secondsToLive, bool burnOnRead) {
    std::string id = crypto.generateId();

    NoteData data;
    data.id = id;
    data.sender = from;
    data.burnOnRead = burnOnRead;
    data.content = crypto.encrypt(text);

    // Zamiana secondsToLive na timestamp ttl (używany przez CleanupService)
    auto now = std::chrono::system_clock::now();
    int64_t seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    data.expirationTime = seconds + secondsToLive;

    return storage.saveFile(to, data);
}

bool NoteManager::deleteNote(std::string user, std::string id) {
    return storage.deleteById(user, id);
}


std::vector<NoteData> NoteManager::getInbox(std::string username) {
    std::vector<NoteData> notes = storage.readFiles(username);

    for (auto& note : notes) {
        note.content = crypto.decrypt(note.content);

        // Obsługa burnOnRead
        if (note.burnOnRead) {
            storage.deleteById(username, note.id);
        }
    }
    return notes;
}