#include "utils.h"

ssize_t getLineFromFile(FILE *fp, char *line, size_t len)
{
    if (fgets(line, len, fp) == NULL)
        return -1;
    
    return strlen(line);
}

void writeLineToFile(char *filepath, char *line)
{
    int fd = open(filepath, O_CREAT, O_WRONLY, O_APPEND, 0777);
    if (fd < 0)
        perror("ERROR can't open the file because ");

    int ret = write(fd, line, strlen(line));
    if (ret < 0)
        perror("ERROR can't write to the file because ");
}

void readConfigFile(char *inputFile, int *connections, char *root, char *index, char *port)
{
	FILE *configFile = fopen(inputFile, "r");
	if (configFile == NULL)		//failed to open file
	{
		perror("Couldn't open configFile ");
		exit(1);
	}

	char *line = (char *)malloc(sizeof(char) * 100);
	size_t len = 100;

	if (getLineFromFile(configFile, line, len) != -1)		//read # connections
	{
		sscanf(line, "%d", *&connections);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read rootDirectory
	{
		strcpy(root, line);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read indexFileName
	{
		strcpy(index, line);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read port number
	{
		strcpy(port, line);
	}	

	free(line);
}

int passivesock(const char *service, const char *transport, int connections)
/*
 * Arguments:
 *      service     - service associated with the desired port
 *      transport   - transport protocol to use ("tcp" or "udp")
 *      connections - number of simultaneous connections
 */
{
	struct servent	*pse;	/* pointer to service information entry	*/
	struct protoent *ppe;	/* pointer to protocol information entry*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, type;	/* socket descriptor and socket type	*/

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

    /* Map service name to port number */
	if ( pse = getservbyname(service, transport) )
		sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
	else if ((sin.sin_port=htons((unsigned short)atoi(service))) == 0)
		perror("can't get service entry because ");

    /* Map protocol name to protocol number */
	if ( (ppe = getprotobyname(transport)) == 0)
		perror("can't get protocol entry because ");

    /* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

    /* Allocate a socket */
	s = socket(PF_INET, type, ppe->p_proto);
	if (s < 0)
		perror("can't create socket because ");

	/* set SO_REUSEADDR on a socket to true (1): */
	int optval = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    /* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		perror("can't bind to port because ");

	if ((listen(s, connections)) != 0)
    {
        perror("Listen failed because ");
        exit(2);
    }

	return s;
}

void readHandler(int s)
{
	ssize_t ssize;
	char buffer[1000];

	while ((ssize = read(s, &buffer, sizeof(buffer))) > 0)		//while we still read data from the client
	{
		char *token = strtok(buffer, " ");

		if (strcmp(token, "GET") == 0)		//GET request received
		{
			token = strtok(NULL, " ");		//get filename

			if (strcmp(token, "/") == 0)	//get home.htm or default
			{
				FILE *file;

				if ((file = fopen("html/index.html", "r")) != NULL)
				{
					//write header
					const char *header = "HTTP/1.1 200 OK\n\n";
					if (send(s, header, sizeof(header), 0) != sizeof(header))
					{
						perror("Header write failed because ");
						exit(4);
					}
					printf("%s", header);

					const int SIZE = 1024;
					char fileBuffer[SIZE];
					while (fgets(fileBuffer, SIZE, file) != NULL)
					{
						printf("%s", fileBuffer);
						if (send(s, fileBuffer, sizeof(fileBuffer), 0) != sizeof(fileBuffer))
						{
							perror("HTML File write failed because ");
							exit(4);
						}
					}
				} 
				else
				{
					//error 404
					const char *error = "HTTP/1.1 404 Not Found\n\n";

					if (write(s, error, sizeof(error)) != sizeof(error))
					{
						perror("Error 404 write failed because ");
						exit(4);
					}
				}
			}
		}

		//strtok to find file name
		//open filename
		//if successfull, write header 
		// HTTP/1.1 200 OK
		// <CR> <CR>
		//read from filename/open
		//write to socket
	}
}