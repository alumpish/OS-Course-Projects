#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <regex>
#include <vector>

#include "logger.hpp"

// using namespace std::string_literals;
namespace fs = std::filesystem;

Logger lg("main");

int get_directory(std::string path, std::vector<std::string>& files) {
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        std::filesystem::path directory_path = path;
        for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename());
            }
        }
    }
    else {
        lg.error("Could not open directory: "s + path);
        return 1;
    }
    return 0;
}