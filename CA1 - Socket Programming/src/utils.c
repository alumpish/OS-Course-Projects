#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ansi_colors.h"
#include "logger.h"

void cliPrompt() {
    write(STDOUT_FILENO, ANSI_WHT ">> " ANSI_RST, 12);
}

void errnoPrint() {
    logError(strerror(errno));
}

int writeToFile(const char* filename, const char* ext, const char* txt) {
    char fname[BUF_NAME + 10] = {'\0'};
    strcpy(fname, filename);
    if (ext != NULL) strcat(fname, ext);

    chmod(fname, S_IWUSR | S_IRUSR);
    int fd = open(fname, O_CREAT | O_WRONLY | O_APPEND);
    if (fd < 0) return 1;

    if (write(fd, txt, strlen(txt)) < 0) return 1;
    close(fd);
    return 0;
}

void printNum(int fd, int num) {
    char buffer[12] = {'\0'};
    snprintf(buffer, 12, "%d", num);
    write(fd, buffer, strlen(buffer));
}

void getInput(int fd, const char* prompt, char* dst, size_t dstLen) {
    if (prompt != NULL) logInput(prompt);
    int cread = read(fd, dst, dstLen);
    if (cread <= 0) {
        errnoPrint();
        exit(EXIT_FAILURE);
    }
    dst[cread - 1] = '\0';
}
