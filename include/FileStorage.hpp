#pragma once

#include <string>
#include <vector>
#include <NoteData.hpp>

class FileStorage {
private:
    std::string basePath;
public:
    FileStorage(std::string path = "data/vault") : basePath(path) {}
    bool saveFile(std::string recipient, NoteData data);
    std::vector<NoteData> readFiles(std::string user);
    bool deleteFile(std::string path);
};