#include "httplib.h"
#include "NoteManager.hpp"
#include "AuthHandler.hpp"
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

std::string extractToken(const httplib::Request& req) {
    if (req.has_header("Authorization")) {
        return req.get_header_value("Authorization");
    }
    return "";
}

int main() {
    httplib::Server svr;
    NoteManager manager;
    AuthHandler auth("data/users.db");

    if (!svr.set_mount_point("/", "./public")) {
        std::cerr << "Błąd: Folder ./public nie istnieje lub brak dostępu!\n";
        return 1;
    }

    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        try {
            json inputJson = json::parse(req.body);

            // Sprawdzenie czy json zawiera wymagane pola
            if (!inputJson.contains("username") || !inputJson.contains("password")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string username = inputJson["username"];
            std::string password = inputJson["password"];

            if (auth.registerUser(username, password)) {
                res.status = 201;
                responseJson["status"] = "success";
            } else {
                res.status = 400;
                responseJson["error"] = "Używtkownik o podanej nazwie już istnieje";
            }
        } catch (const json::exception& e) {
            res.status = 400;
            responseJson["error"] = "Blad w JSON: " + std::string(e.what());
        }

        res.set_content(responseJson.dump(), "application/json; charset=utf-8");
    });

    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        try {
            json inputJson = json::parse(req.body);

            // Sprawdzenie czy json zawiera wymagane pola
            if (!inputJson.contains("username") || !inputJson.contains("password")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string username = inputJson["username"];
            std::string password = inputJson["password"];

            std::string token = auth.login(username, password);

            if (!token.empty()) {
                res.status = 201;
                responseJson["status"] = "success";
                responseJson["token"] = token;
            } else {
                res.status = 401;
                responseJson["error"] = "Niepoprawny login lub hasło";
            }
        } catch (const json::exception& e) {
            res.status = 400;
            responseJson["error"] = "Blad w JSON: " + std::string(e.what());
        }

        res.set_content(responseJson.dump(), "application/json; charset=utf-8");
    });

    svr.Post("/send", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        try {
            json inputJson = json::parse(req.body);

            std::string token = extractToken(req);
            std::string from = auth.authenticate(token);

            if (from.empty()) {
                res.status = 401;
                responseJson["error"] = "Brak tokenu lub token wygasł. Zaloguj się ponownie";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            // Sprawdzenie czy json zawiera wymagane pola
            if (!inputJson.contains("to") || !inputJson.contains("text")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string to = inputJson["to"];
            std::string text = inputJson["text"];

            if (manager.sendNote(from, to, text, 3600, false)) {
                res.status = 201;
                responseJson["status"] = "success";
            } else {
                res.status = 500;
                responseJson["error"] = "Bład zapisu w StorageManager";
            }
        } catch (const json::exception& e) {
            res.status = 400;
            responseJson["error"] = "Blad w JSON: " + std::string(e.what());
        }

        res.set_content(responseJson.dump(), "application/json; charset=utf-8");
    });

    svr.Get("/inbox", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;

        std::string token = extractToken(req);
        std::string user = auth.authenticate(token);

        if (user.empty()) {
            res.status = 401;
            responseJson["error"] = "Brak tokenu lub token wygasł. Zaloguj się ponownie";
            res.set_content(responseJson.dump(), "application/json; charset=utf-8");
            return;
        }
        
        auto notes = manager.getInbox(user);
        
        // Utowrzenie tablicy JSON
        json responseArray = json::array();
        
        for (const auto& note : notes) {
            json noteJson;
            noteJson["sender"] = note.sender;
            noteJson["content"] = note.content;
            responseArray.push_back(noteJson);
        }
        
        res.set_content(responseArray.dump(), "application/json; charset=utf-8");
    });

    std::cout << "Serwer nasluchuje na porcie 8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}