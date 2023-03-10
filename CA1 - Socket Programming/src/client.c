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
ClientType getClientType(int server_fd) {
    logNormal("Which are you? (1-Student, 2-TA)");
    while (1) {
        char cmdBuf[BUF_CLI] = {'\0'};
        char msgBuf[BUF_MSG] = {'\0'};
        ClientType client_type;

        getInput(STDIN_FILENO, NULL, cmdBuf, BUF_CLI);

        if (strcmp(cmdBuf, "1") == 0) {
            client_type = STUDENT;
            snprintf(msgBuf, BUF_MSG, "$STU$");
            send(server_fd, msgBuf, strlen(msgBuf), 0);
            return client_type;
        }
        else if (strcmp(cmdBuf, "2") == 0) {
            client_type = TA;
            snprintf(msgBuf, BUF_MSG, "$TAA$");
            send(server_fd, msgBuf, strlen(msgBuf), 0);
            return client_type;
        }
        else {
            logError("Invalid input");
            continue;
        }
    }
}

void cli(fd_set* master_set, BroadcastInfo* br_info, int server_fd, ClientType client_type) {
    char cmdBuf[BUF_CLI] = {'\0'};
    char msgBuf[BUF_MSG] = {'\0'};
    // memset(msgBuf, 0, 1024);
    // memset(cmdBuf, 0, 1024);

    getInput(STDIN_FILENO, NULL, cmdBuf, BUF_CLI);
    char* cmdPart = strtok(cmdBuf, " ");
    if (cmdPart == NULL)
        return;

    if (br_info->fd != -1) {
        snprintf(msgBuf, BUF_MSG, "%d %d %d", br_info->fd, br_info->addr.sin_port, br_info->addr.sin_addr.s_addr, br_info->addr.sin_family);

        sendto(br_info->fd, cmdPart, strlen(cmdPart), 0, (struct sockaddr*)&(br_info->addr), sizeof(br_info->addr));
        logInfo(msgBuf);
        logInfo(cmdPart);

        return;
    }

    if (!strcmp(cmdPart, "help")) {
        if (client_type == STUDENT)
            logNormal(
                "Available commands:\n"
                " - ask <question>: send a question to server.\n"
                " - show_sessions: show progressing sessions\n"
                " - connect <port>: connect to a TA.");
        else if (client_type == TA)
            logNormal(
                "Available commands:\n"
                " - show_questions: show list of all available questions.\n"
                " - answer <question_id>: choose a question to discuss about it.\n"
                " - connect <port>: connect to a student.");
    }
    else if (!strcmp(cmdPart, "ask")) {
        char* cmdPart = strtok(NULL, " ");
        if (cmdPart == NULL) {
            logError("No question provided");
            return;
        }

        snprintf(msgBuf, BUF_MSG, "$ASK$%s", cmdPart);
        send(server_fd, msgBuf, strlen(msgBuf), 0);
        // alarm(TIMEOUT);
    }
    else if (!strcmp(cmdPart, "show_sessions")) {
        snprintf(msgBuf, BUF_MSG, "$SSN$");
        send(server_fd, msgBuf, strlen(msgBuf), 0);
        // alarm(TIMEOUT);
        logInfo("Session request sent.");
    }
    else if (!strcmp(cmdPart, "show_questions")) {
        snprintf(msgBuf, BUF_MSG, "$SQN$");
        send(server_fd, msgBuf, strlen(msgBuf), 0);
        // alarm(TIMEOUT);
        // logInfo("Q request sent.");
    }
    else if (!strcmp(cmdPart, "answer")) {
        char* cmdPart = strtok(NULL, " ");
        if (cmdPart == NULL) {
            logError("No answer provided");
            return;
        }

        snprintf(msgBuf, BUF_MSG, "$ANS$%s", cmdPart);
        send(server_fd, msgBuf, strlen(msgBuf), 0);
        // alarm(TIMEOUT);
    }
    else if (!strcmp(cmdPart, "connect")) {
        char* cmdPart = strtok(NULL, " ");
        if (cmdPart == NULL) {
            logError("No port provided");
            return;
        }
        int port, res;
        res = strToInt(cmdPart, &port);
        if (res == 1 || res == 2) {
            logError("Invalid question id");
            return;
        }

        *br_info = initBroadcastSocket(port);
        FD_SET(br_info->fd, master_set);
        snprintf(msgBuf, BUF_MSG, "br_fd %d", br_info->fd);
        logInfo(msgBuf);

        // alarm(TIMEOUT);
    }
    else {
        logError("Invalid command.");
    }
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket, max_sd;
    int broadcast_fd = -1;

    char buffer[1024] = {0};

    BroadcastInfo brInfo;
    memset(&brInfo, 0, sizeof(brInfo));
    brInfo.fd = -1;

    server_fd = connectServer(8080);

    ClientType client_type = getClientType(server_fd);

    fd_set master_set, working_set;

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(STDIN_FILENO, &master_set);
    FD_SET(server_fd, &master_set);

    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                snprintf(buffer, 1024, "fd = %d", i);
                logInfo(buffer);

                if (i != STDIN_FILENO) {
                    write(STDOUT_FILENO, "\x1B[2K\r", 5);
                }
                if (i == STDIN_FILENO) {
                    cli(&master_set, &brInfo, server_fd, client_type);
                }
                else if (i == server_fd) {
                    logInfo("mew");
                    int bytes_received;
                    memset(buffer, 0, 1024);
                    bytes_received = recv(i, buffer, 1024, 0);

                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    if (!strncmp(buffer, "$PRM$", 5)) {
                        logError("Permission Denied!");
                    }
                    else if (!strncmp(buffer, "$PRT$", 5)) {
                        int port, res;
                        char* port_str = buffer + 5;
                        res = strToInt(port_str, &port);
                        printf("port = %d\n", port);
                    }
                    else {
                        printf(buffer);
                    }
                }
                else if (i == broadcast_fd) {
                    int bytes_received;
                    memset(buffer, 0, 1024);
                    bytes_received = recv(i, buffer, 1024, 0);

                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    printf(buffer);
                }
                else if (i == brInfo.fd) {
                    logInfo("Receiving");

                    int bytes_received;
                    memset(buffer, 0, 1024);
                    bytes_received = recv(i, buffer, 1024, 0);

                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    printf(buffer);
                }
            }
        }
    }

    return 0;
}