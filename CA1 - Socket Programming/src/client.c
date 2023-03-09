#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "logger.h"
#include "types.h"
#include "utils.h"

int connectServer(int port) {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket, max_sd;
    ClientType client_type;

    server_fd = connectServer(8080);
    printf("%d", server_fd);


    char cmdBuf[BUF_CLI] = {'\0'};
    char msgBuf[BUF_MSG] = {'\0'};

    logNormal("Which are you? (1-Student, 2-TA)");
    getInput(STDIN_FILENO, NULL, cmdBuf, BUF_CLI);

    if (strcmp(cmdBuf, "1") == 0) {
        client_type = STUDENT;
        snprintf(msgBuf, BUF_MSG, "$STU$");
        send(server_fd, msgBuf, strlen(msgBuf), 0);
    }
    else if (strcmp(cmdBuf, "2") == 0) {
        client_type = TA;
        snprintf(msgBuf, BUF_MSG, "$TAA$");
        send(server_fd, msgBuf, strlen(msgBuf), 0);
    }
    else {
        logError("Invalid input");
        return 1;
    }

    fd_set master_set, working_set;

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(server_fd, &master_set);



    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (i != STDIN_FILENO) {
                write(STDOUT_FILENO, "\x1B[2K\r", 5);
            }
            if (i == STDIN_FILENO) {
                memset(cmdBuf, 0, 1024);
                getInput(STDIN_FILENO, NULL, cmdBuf, BUF_CLI);
                char* cmdPart = strtok(cmdBuf, " ");
                if (cmdPart == NULL)
                    continue;

                if (!strcmp(cmdPart, "help")) {
                    if (client_type == STUDENT)
                        logNormal(
                            "Available commands:\n"
                            " - ask <question>: send a question to server.\n"
                            " - show_sessions: show progressing sessions\n");
                    else if (client_type == TA)
                        logNormal(
                            "Available commands:\n"
                            " - show_questions: show list of all available questions.\n"
                            " - answer <question_id>: choose a question to discuss about it.\n");
                }
                else if (!strcmp(cmdPart, "ask")) {
                    char* cmdPart = strtok(NULL, " ");
                    if (cmdPart == NULL) {
                        logError("No question provided");
                        continue;
                    }

                    snprintf(msgBuf, BUF_MSG, "$ASK$%s", cmdPart);
                    send(server_fd, msgBuf, strlen(msgBuf), 0);
                    // alarm(TIMEOUT);
                    logInfo("Question sent.");
                }
                else if (!strcmp(cmdPart, "show_sessions")) {
                    snprintf(msgBuf, BUF_MSG, "$SSN$");
                    send(server_fd, msgBuf, strlen(msgBuf), 0);
                    // alarm(TIMEOUT);
                    logInfo("Session request sent.");
                }
                else if (!strcmp(cmdPart, "show_questions")) {
                    // cmdPart = strtok(NULL, "");
                    snprintf(msgBuf, BUF_MSG, "$SQN$");
                    send(server_fd, msgBuf, strlen(msgBuf), 0);
                    // alarm(TIMEOUT);
                    logInfo("Q request sent.");
                }
                else if (!strcmp(cmdPart, "answer")) {
                    // char* cmdPart = strtok(NULL, " ");
                    if (cmdPart == NULL) {
                        logError("No answer provided");
                        continue;
                    }

                    cmdPart = strtok(NULL, "");
                    snprintf(msgBuf, BUF_MSG, "$ANS$%s", cmdPart);
                    send(server_fd, msgBuf, strlen(msgBuf), 0);
                    // alarm(TIMEOUT);
                    logInfo("Answer sent.");
                }
                else {
                    logError("Invalid command.");
                }

                // printf("%s\n", cmdBuf);
                // memset(cmdBuf, 0, 1024);
            }
        }
    }

    return 0;
}