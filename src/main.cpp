#include "httplib.h"
#include "json.hpp"
#include "NoteManager.hpp"
#include "AuthHandler.hpp"
#include "CleanupService.hpp"
#include "LoadEnv.hpp"
#include <iostream>
#include <chrono>
#include <format>

using json = nlohmann::json;

std::string extractToken(const httplib::Request& req) {
    if (req.has_header("Authorization")) {
        return req.get_header_value("Authorization");
    }
    return "";
}

int main() {
    std::map<std::string, std::string> env = loadEnv(".env");

    if (env.find("JWT_SECRET") == env.end() || env.find("CRYPTO_KEY") == env.end()) {
        std::cerr << "[BLAD] Brak wymaganych kluczy w pliku .env! Serwer zatrzymany.\n";
        return 1;
    }

    httplib::Server svr;
    NoteManager manager(env["CRYPTO_KEY"]);
    FileStorage& storage = manager.getStorage();
    AuthHandler auth("data/users.db", env["JWT_SECRET"]);
    CleanupService cleanup(storage, 30);

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
            responseJson["error"] = "Nieprawidłowy format JSON";
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
            responseJson["error"] = "Nieprawidłowy format JSON";
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
            if (!inputJson.contains("to") || !inputJson.contains("text") || !inputJson.contains("ttl") || !inputJson.contains("burn")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string to = inputJson["to"];
            std::string text = inputJson["text"];
            int ttl = inputJson["ttl"];
            bool burnOnRead = inputJson["burn"];

            if (manager.sendNote(from, to, text, ttl, burnOnRead)) {
                res.status = 201;
                responseJson["status"] = "success";
            } else {
                res.status = 500;
                responseJson["error"] = "Wystąpił błąd podczas zapisu";
            }
        } catch (const json::exception& e) {
            res.status = 400;
            responseJson["error"] = "Nieprawidłowy format JSON";
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
            noteJson["id"] = note.id;
            noteJson["sender"] = note.sender;
            noteJson["content"] = note.content;
            noteJson["burn"] = note.burnOnRead;

            if (note.expirationTime > 0) {
                std::chrono::sys_seconds timePoint{std::chrono::seconds(note.expirationTime)};
                auto localTimePoint = std::chrono::current_zone()->to_local(timePoint);
                std::string date = std::format("{:%Y-%m-%d %H:%M:%S}", localTimePoint);
                noteJson["ttl"] = date;
            }

            responseArray.push_back(noteJson);
        }
        
        res.set_content(responseArray.dump(), "application/json; charset=utf-8");
    });

    svr.Get("/username", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        std::string token = extractToken(req);
        std::string user = auth.authenticate(token);

        if (user.empty()) {
            res.status = 401;
            responseJson["error"] = "Brak tokenu lub token wygasł. Zaloguj się ponownie";
            res.set_content(responseJson.dump(), "application/json; charset=utf-8");
            return;
        }
        responseJson["username"] = user;
        res.set_content(responseJson.dump(), "application/json; charset=utf-8");
    });

    if (!svr.set_mount_point("/", "./public")) {
        std::cerr << "Błąd: Folder ./public nie istnieje lub brak dostępu!\n";
        return 2;
    }

svr.Delete("/delete", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        try {
            json inputJson = json::parse(req.body);

            std::string token = extractToken(req);
            std::string user = auth.authenticate(token);

            if (user.empty()) {
                res.status = 401;
                responseJson["error"] = "Brak tokenu lub token wygasł. Zaloguj się ponownie";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            // Sprawdzenie czy json zawiera wymagane pola
            if (!inputJson.contains("id")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string id = inputJson["id"];

            if (manager.deleteNote(user, id)) {
                res.status = 200;
                responseJson["status"] = "success";
            } else {
                res.status = 500;
                responseJson["error"] = "Wystąpił błąd podczas usuwania";
            }
        } catch (const json::exception& e) {
            res.status = 400;
            responseJson["error"] = "Nieprawidłowy format JSON";
        }

        res.set_content(responseJson.dump(), "application/json; charset=utf-8");
    });

    cleanup.start();

    std::cout << "Serwer nasluchuje na porcie 8080\n";
    svr.listen("0.0.0.0", 8080);

    cleanup.stop();
    return 0;
}