#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "logger.h"
#include "types.h"
#include "utils.h"

int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}

int main(int argc, char const *argv[])
{
    int fd;
    Client client;

    fd = connectServer(8080);

    char cmdBuf[BUF_CLI] = {'\0'};

    logNormal("Which are you? (1-Student, 2-TA)");
    getInput(STDIN_FILENO, NULL, cmdBuf, BUF_CLI);

    if (strcmp(cmdBuf, "1") == 0)
    {
        send(fd, "$STU$", 5, 0);
    }
    else if (strcmp(cmdBuf, "2") == 0)
    {
        send(fd, "$TA$", 4, 0);
        memset(cmdBuf, 0, 1024);
    }
    else
    {
        logError("Invalid input");
        return 1;
    }
    
    memset(cmdBuf, 0, 1024);

    while(1)
    {
        recv(fd, cmdBuf, BUF_CLI, 0);

        printf("%s\n", cmdBuf);
        memset(cmdBuf, 0, 1024);
    }

    return 0;
}