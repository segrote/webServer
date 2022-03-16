#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 2) // argument handling
    {
        printf("Usage: ./server [conf/configfile.conf]\n");
        fflush(stdout);
        exit(1);
    }

	int simultaneousConnections, sockfd, connfd, len;
	char portNumber[10];
    char *inputFile = argv[1];

	readConfigFile(inputFile, &simultaneousConnections, portNumber);

	if ((sockfd = passivesock(portNumber, "tcp", simultaneousConnections)) == -1)
		exit(2);

    struct sockaddr_in cli;
    len = sizeof(cli);
    ssize_t ssize;
    bzero(&cli, len);
    pthread_t tid;

	while(1)
	{
        connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
        if (connfd < 0)
        {
            perror("Server accept failed because ");
            exit(3);
        }
            
        if (pthread_create(&tid, NULL, readHandler, (void*)&connfd) < 0)
        {
            perror("Thread creation failed because ");
            exit(4);
        }
        // readHandler(connfd);
    }

    close(sockfd);      //server never shut down

	return 0;
}