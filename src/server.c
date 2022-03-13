#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 2) // argument handling
    {
        printf("Usage: ./server [conf/configfile.conf]\n");
        exit(1);
    }

	int simultaneousConnections, sockfd, connfd, len;
	char rootDirectory[100], indexFileName[100], portNumber[10], buffer[512];
    char *inputFile = argv[1];

	readConfigFile(inputFile, &simultaneousConnections, rootDirectory, indexFileName, portNumber);

	if ((sockfd = passivesock(portNumber, "tcp", simultaneousConnections)) == -1)
		exit(2);

    struct sockaddr_in cli;
    len = sizeof(cli);
    ssize_t ssize;
    bzero(&cli, len);

	while(1)
	{
        connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
        if (connfd < 0)
        {
            perror("Server accept failed because ");
            exit(3);
        }
            
        readHandler(connfd);
    }

    close(sockfd);      //server never shut down

	return 0;
}