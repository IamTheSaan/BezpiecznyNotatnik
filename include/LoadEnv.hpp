#pragma once

#include <string>
#include <map>

std::map<std::string, std::string> loadEnv(const std::string& filepath = ".env");