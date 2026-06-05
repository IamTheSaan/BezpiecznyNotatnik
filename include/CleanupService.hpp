#pragma once
#include <thread>
#include <atomic>
#include "FileStorage.hpp"

class CleanupService {
private:
    FileStorage& storage; // referencja
    int checkInterval;          
    std::thread workerThread;   
    std::atomic<bool> running;  

    void run();
public:
    CleanupService(FileStorage& storage, int intervalSeconds = 60) : storage(storage), checkInterval(intervalSeconds), running(false) {}
    ~CleanupService() {stop();}

    void start();
    void stop();
};