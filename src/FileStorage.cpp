#include <iostream>
#include <filesystem>
#include <fstream>
#include "FileStorage.hpp"
#include "base64.h"

namespace fs = std::filesystem;

bool FileStorage::saveFile(const std::string& recipient, const NoteData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        // Tworzymy ścieżkę do folderu użytkownika
        fs::path userPath = fs::path(basePath) / recipient;

        // Jeśli folder nie istnieje to go tworzymy
        if (!fs::exists(userPath)) {
            fs::create_directories(userPath);
        }

        std::string fileName = data.id + ".enc";
        fs::path filePath = userPath / fileName;

        //Otwieramy plik do zapisu
        std::ofstream outFile(filePath);
        if(!outFile.good()){
            std::cerr << "Plik sie nie otworzyl\n";
            return false;
        }
        outFile << data.expirationTime << "\n"; // zapisujemy ttl w pierwszej linijsce aby ułatwić pracę CleanupService
        outFile << data.id << "\n";
        outFile << data.sender << "\n";
        outFile << data.burnOnRead << "\n";
        outFile << base64_encode(data.content);
        outFile.close();
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas zapisu: " << e.what() << "\n";
        return false;
    }
}

std::vector<NoteData> FileStorage::readFiles(const std::string& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<NoteData> notes;
    try {
        fs::path userPath = fs::path(basePath) / user;
        if (!fs::exists(userPath))return notes;

        for (const auto& entry : fs::directory_iterator(userPath)) {
            if (entry.is_regular_file()) {
                std::ifstream inFile(entry.path());
                if (!inFile.is_open()) continue;

                NoteData data;
                std::string line;
                std::getline(inFile, line);
                data.expirationTime = std::stoll(line);
                std::getline(inFile, data.id);
                std::getline(inFile, data.sender);
                std::getline(inFile, line);
                if(line=="1") data.burnOnRead=true;
                else data.burnOnRead=false;
                std::getline(inFile, line);
                data.content = base64_decode(line);

                notes.push_back(data);
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas odczytu: " << e.what() << "\n";
    }

    return notes;
}

bool FileStorage::deleteById(const std::string& user, const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        fs::path path = fs::path(basePath) / user / (id + ".enc");
        return fs::remove(path);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas usuwania pliku: " << e.what() << "\n";
        return false;
    }
}

void FileStorage::cleanup(int64_t currentTimestamp) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        if (!fs::exists(basePath)) return;
        int deletedCount = 0;

        for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".enc") {
                std::ifstream inFile(entry.path());
                if (!inFile.is_open()) continue;

                std::string line;
                // czytamy ttl
                if (std::getline(inFile, line)) {
                    inFile.close(); // zamykamy plik przed ewentualnym usunięciem
                    try {
                        int64_t expirationTime = std::stoll(line);

                        if (expirationTime <= currentTimestamp && expirationTime > 0) {
                            if (fs::remove(entry.path())) {
                                deletedCount++;
                            }
                        }
                    }
                    catch (const std::exception& e) {
                        std::cerr << "[Cleanup] Blad: " << e.what() << "\n";
                    }
                } else {
                    inFile.close();
                }
            }
        }

        if (deletedCount > 0) {
            std::cout << "[Cleanup] Usunieto z dysku wygasle notatki: " << deletedCount << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Cleanup] Blad: " << e.what() << "\n";
    }
}