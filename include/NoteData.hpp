#pragma once

#include <string>
#include <cstdint>

struct NoteData {
    std::string sender;
    std::string content;
    bool burnOnRead;
    int64_t expirationTime;
};