#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include "string.h"
#include "fcntl.h"
#include <stdio.h>
#include <netdb.h>
#include "stdlib.h"

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff
#endif	/* INADDR_NONE */

ssize_t getLineFromFile(FILE *fp, char *line, size_t len);
void writeLineToFile(char *filepath, char *line);
void readConfigFile(char *inputFile, int *connections, char *root, char *index, char *port);
int passivesock(const char *service, const char *transport, int connections);