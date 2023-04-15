#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "consts.hpp"
#include "csv.hpp"
#include "logger.hpp"
#include "strutils.hpp"

using namespace std::string_literals;
namespace fs = std::filesystem;

Logger lg("reduce");

bool isInPosWanted(std::string pos, const std::vector<std::string>& pos_wanted) {
    for (auto& p : pos_wanted) {
        if (p == pos)
            return true;
    }
    return false;
}

int main(int argc, const char* argv[]) {
    if (argc != 4) {
        lg.error("Wrong number of arguments");
        return 1;
    }

    int fd0 = atoi(argv[2]);
    int fd1 = atoi(argv[3]);

    close(fd1);
    char buffer[consts::BUFF_SIZE];
    int bytes_read = read(fd0, buffer, consts::BUFF_SIZE);
    close(fd0);
    if (bytes_read == -1) {
        lg.error("Can't read from pipe");
        return 1;
    }

    std::vector<std::string> pos_wanted = strutils::split(buffer, " ");

    // read argv[1] csv file
    Csv csv(argv[1]);
    csv.readfile();
    auto tbl = csv.get();

    // get number of parts
    int parts_count = 0;
    for (auto row : tbl) {
        if (isInPosWanted(row[1], pos_wanted)) {
            // add row[1], row[2] to 
        }
    }

    // get genre
    string genre = "";
    for (auto row : tbl) {
        if (row[0] == "genre") {
            genre = row[1];
            break;
        }
    }

    for (string pos : pos_wanted) {
        for (int i = 1; i <= parts_count; i++) {
            string pipe_name = PIPES_PATH + "/" + genre + "_" + to_string(i);
            if (mkfifo(pipe_name.c_str(), 0666) == -1) {
                cerr << "Could not create pipe" << endl;
                return 1;
            }
        }
    }

    return EXIT_SUCCESS;
}