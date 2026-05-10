#include <iostream>
#include <filesystem>
#include <fstream>
#include <FileStorage.hpp>

namespace fs = std::filesystem;

bool FileStorage::saveFile(std::string recipient, NoteData data) {
    try {
        // Tworzymy ścieżkę do folderu użytkownika
        fs::path userPath = fs::path(basePath) / recipient;

        // Jeśli folder nie istnieje to go tworzymy
        if (!fs::exists(userPath)) {
            fs::create_directory(userPath);
        }

        std::string fileName = data.id + ".bin";
        fs::path filePath = userPath / fileName;

        //Otwieramy plik do zapisu
        std::ofstream outFile(filePath);
        //Masz to w binary otwarte a potem wpisujesz znak \n i to sie gryzie
        if(!outFile.good()){
            std::cerr << "Plik sie nie otworzyl\n";
            return false;
        }
        outFile << data.id << "\n";
        outFile << data.sender << "\n";
        outFile << data.expirationTime << "\n";
        outFile << data.burnOnRead << "\n";
        outFile << data.content;
        outFile.close();
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas zapisu: " << e.what() << "\n";
        return false;
    }
}

std::vector<NoteData> FileStorage::readFiles(std::string user) {
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
                std::getline(inFile, data.id);
                std::getline(inFile, data.sender);
                std::getline(inFile, line);
                data.expirationTime = std::stoll(line);
                std::getline(inFile, line);
                if(line=="1") data.burnOnRead=true;
                else data.burnOnRead=false;
                std::string content, temp;
                while (std::getline(inFile, temp)) {
                    content += temp;
                    if (!inFile.eof()) content += "\n";
                }
                data.content = content;

                notes.push_back(data);
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas odczytu: " << e.what() << "\n";
    }

    return notes;
}

bool FileStorage::deleteFile(std::string path) {
    try {
        return fs::remove(path);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas usuwania pliku: " << e.what() << "\n";
        return false;
    }
}

bool FileStorage::deleteById(std::string user, std::string id) {
    try {
        fs::path path = fs::path(basePath) / user / (id + ".bin");
        return fs::remove(path);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Blad podczas usuwania pliku: " << e.what() << "\n";
        return false;
    }
}