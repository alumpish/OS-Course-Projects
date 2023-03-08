#ifndef TYPES_H
#define TYPES_H

#include <netinet/in.h>
#include <sys/select.h>

#define BUF_NAME  32
#define BUF_PNAME 64
#define BUF_CLI   128
#define BUF_MSG   512
#define BCAST_IP  "192.168.1.255"
#define TIMEOUT   60


typedef enum {
    STUDENT = 0,
    TA = 1
} ClientType;

typedef enum {
    WAITING = 0,
    DISCUSS = 1,
    EXPIRED = 2
} QuestionType;

typedef struct {
    ClientType type;
} Client;

typedef struct {
    int fd;
    struct sockaddr_in addr;
} BroadcastInfo;

typedef struct {
    QuestionType type;
    char qMsg[BUF_MSG];
    char aMsg[BUF_MSG];
    BroadcastInfo bcast;
};

#endif TYPES_H
