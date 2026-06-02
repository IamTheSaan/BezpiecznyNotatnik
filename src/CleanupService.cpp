#include "CleanupService.hpp"
#include <chrono>
#include <iostream>

void CleanupService::start() {
    if (!running) {
        running = true;
        workerThread = std::thread(&CleanupService::run, this);
        std::cout << "[Cleanup] Serwis sprzatajacy uruchomiony.\n";
    }
}

void CleanupService::stop() {
    if (running) {
        running = false;
        if (workerThread.joinable()) {
            workerThread.join();
            std::cout << "[Cleanup] Serwis sprzatajacy zatrzymany.\n";
        }
    }
}

void CleanupService::run() {
    while (running) {
        auto now = std::chrono::system_clock::now();
        int64_t seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        storage.cleanup(seconds); 

        for (int i = 0; i < checkInterval && running; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}