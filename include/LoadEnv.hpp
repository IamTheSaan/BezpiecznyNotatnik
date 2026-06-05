#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>

// Funkcja czytająca plik .env i zwracająca słownik klucz-wartość
std::map<std::string, std::string> loadEnv(const std::string& filepath = ".env") {
    std::map<std::string, std::string> env;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[OSTRZEZENIE] Nie udalo sie otworzyc pliku " << filepath << ". Upewnij sie, ze istnieje.\n";
        return env;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Szukamy znaku równości, który oddziela klucz od wartości
        size_t delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            
            env[key] = value;
        }
    }
    file.close();
    return env;
}