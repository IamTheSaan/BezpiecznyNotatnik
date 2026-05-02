#include <iostream>
#include <FileStorage.hpp>
#include <NoteData.hpp>
#include <string>

int main() {
    FileStorage storage("../data/vault");
    std::string user = "piotr";
    std::string id = "1234";
    NoteData note;
    note.sender = "samuel";
    note.content = "tajna wiadomosc";
    note.burnOnRead = true;
    note.expirationTime = 123456;

    storage.saveFile(user, id, note);
    return 0;
}