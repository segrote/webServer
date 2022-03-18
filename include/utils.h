#ifndef UTILS_H_
#define UTILS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>

char rootDirectory[100], indexFileName[100];

ssize_t getLineFromFile(FILE *fp, char *line, size_t len);
void writeLineToFile(char *filepath, char *line);
void readConfigFile(char *inputFile, int *connections, char *port);
int passivesock(const char *service, const char *transport, int connections);
void *readHandler(void *arg);
void accessLog(char *header);

#endif