#pragma once

#include <string>
#include <vector>
#include <NoteData.hpp>

class FileStorage {
private:
    std::string basePath;
public:
    FileStorage(const std::string& path = "data/vault") : basePath(path) {}
    bool saveFile(const std::string& recipient, const NoteData& data);
    std::vector<NoteData> readFiles(const std::string& user);
    void cleanup(int64_t curretTimestamp);
    bool deleteById(const std::string& user, const std::string& id);
    std::string getBasePath() { return basePath; } // do testów
};