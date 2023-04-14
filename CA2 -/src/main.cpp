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

using namespace std::string_literals;
namespace fs = std::filesystem;

Logger lg("main");

int get_directory_folders(std::string path, std::vector<fs::path>& folders) {
    if (fs::exists(path) && fs::is_directory(path)) {
        fs::path directory_path = path;
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (entry.is_directory()) {
                folders.push_back(entry.path());
            }
        }
    }
    else {
        lg.error("Could not open directory: "s + path);
        return 1;
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << "ClubsAgeStats.out" << " <clubs folder>\n";
        return EXIT_FAILURE;
    }

    std::vector<fs::path> folders;
    fs::path pos_file;

    if (get_directory_folders(argv[1], folders, pos_file))
        return EXIT_FAILURE;

    // fs::path genresPath;
    // if (getRequiredFiles(files, genresPath))
    //     return EXIT_FAILURE;

    // std::vector<std::string> genres;
    // if (readGenres(genresPath, genres))
    //     return EXIT_FAILURE;

    // if (workers(files, genres))
    //     return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
