#pragma once

#include <string>
#include <cstdint>

struct NoteData {
    std::string id;
    std::string sender;
    int64_t expirationTime;
    bool burnOnRead;
    std::string content;
};