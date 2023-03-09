#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE

#include "types.h"

// Print the prompt string.
void cliPrompt();
// Print errno text representation to the standard error output.
void errnoPrint();

// Write txt to filename. Returns 1 on error, 0 on success.
int writeToFile(const char* filename, const char* ext, const char* txt);

// Print a number to file descriptor fd
void printNum(int fd, int num);
void getInput(int fd, const char* prompt, char* dst, size_t dstLen);

void addClient(ClientArray* arr, Client client);

#endif // UTILS_H_INCLUDE
