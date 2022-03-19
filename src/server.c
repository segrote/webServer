#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 2) // argument handling
    {
        printf("Usage: ./server [conf/configfile.conf]\n");
        exit(1);
    }

    //map stdout to access log file and stderr to error log file
    // if (freopen("logs/httpd-access.log", "w", stdout) == NULL || freopen("logs/httpd-error.log", "w", stderr) == NULL)
    // {
    //     printf("Can't map std streams to log file\n");
    //     exit(2);
    // }

	int simultaneousConnections, sockfd, connfd, len;
	char portNumber[10], *inputFile = argv[1];
    bzero(rootDirectory, 100); bzero(indexFileName, 100);

	readConfigFile(inputFile, &simultaneousConnections, portNumber);

	if ((sockfd = passivesock(portNumber, "tcp", simultaneousConnections)) == -1)
    {
        perror("Can't setup passive socket because ");
        fflush(stderr);
        exit(3);
    }

    struct sockaddr_in cli;
    len = sizeof(cli);
    bzero(&cli, len);
    ssize_t ssize;
    pthread_t tid;    

	while(1)
	{
        connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
        if (connfd < 0)
        {
            perror("Server accept failed because ");
            fflush(stderr);
            exit(4);
        }
            
        if (pthread_create(&tid, NULL, readHandler, (void*)&connfd) < 0)
        {
            perror("Thread creation failed because ");
            fflush(stderr);
            exit(5);
        }
    }

    close(sockfd);      //server never shut down

	return 0;
}