#include "httplib.h"
#include "NoteManager.hpp"
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

int main() {
    httplib::Server svr;
    NoteManager manager;

    if (!svr.set_mount_point("/", "./public")) {
        std::cerr << "Błąd: Folder ./public nie istnieje lub brak dostępu!\n";
        return 1;
    }

    svr.Post("/send", [&](const httplib::Request& req, httplib::Response& res) {
        json responseJson;
        try {
            json inputJson = json::parse(req.body);

            // Sprawdzenie czy json zawiera wymagane pola
            if (!inputJson.contains("from") || !inputJson.contains("to") || !inputJson.contains("text")) {
                res.status = 400;
                responseJson["error"] = "Brak wymaganych pól w JSON";
                res.set_content(responseJson.dump(), "application/json; charset=utf-8");
                return;
            }

            std::string from = inputJson["from"];
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
        std::string user = req.get_param_value("user");
        
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