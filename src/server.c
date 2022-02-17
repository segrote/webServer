#include "utils.h"

int main(int argc, char *argv[])
{
    // argument handling
    if(argc != 2)
    {
        printf("Usage: ./server [conf/configfile.conf]\n");
        return 1;
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
            exit(1);
        }
        else   
            printf("Server accept\n");
        
        ssize = recv(connfd, &buffer, sizeof(buffer), 0);
        if (ssize < 0)
        {
            perror("Server receive failed because ");
            exit(1);
        }
        else
        {
            printf("Got: %s\n", buffer);
            printf("Buffer size of: %ld\n", strlen(buffer));
        }
    }

    close(sockfd);      //server never shut down

	return 0;
}