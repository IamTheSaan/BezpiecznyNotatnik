#include "httplib.h"
#include "NoteManager.hpp"
#include <iostream>

int main() {
    httplib::Server svr;
    NoteManager manager;

    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Serwer działa i pozdrawia!", "text/plain; charset=utf-8");
    });

    svr.Post("/send", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("from") && req.has_param("to") && req.has_param("text")) {
            std::string from = req.get_param_value("from");
            std::string to   = req.get_param_value("to");
            std::string text = req.get_param_value("text");

            manager.sendNote(from, to, text, 3600, false); // ttl i burnonread tymczasowo na stałe
            
            res.set_content("Zapisano notatke!", "text/plain");
        } else {
            res.status = 400;
            res.set_content("Blad: Brak danych w Body!", "text/plain");
        }
    });

    svr.Get("/inbox", [&](const httplib::Request& req, httplib::Response& res) {
        std::string user = req.get_param_value("user");
        
        auto notes = manager.getInbox(user);
        
        std::string odpowiedz = "Skrzynka uzytkownika " + user + ":\n";
        for (const auto& n : notes) {
            odpowiedz += "Od [" + n.sender + "]: " + n.content + "\n";
        }
        res.set_content(odpowiedz, "text/plain");
    });

    std::cout << "Serwer nasluchuje na porcie 8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}