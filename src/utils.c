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
	char buffer[1024];

	while ((ssize = read(s, &buffer, sizeof(buffer))) > 0)		//while we still read data from the client
	{
		printf("%s\n", buffer);
		char *token = strtok(buffer, " ");

		if (strcmp(token, "GET") == 0)		//GET request received
		{
			token = strtok(NULL, " ");		//get filename

			if (strcmp(token, "/") == 0)	//get home.htm or default
			{
				FILE *file;

				if ((file = fopen("html/home.htm", "r")) != NULL)
				{
					//write header
					const char *header = "HTTP/1.0 200 OK\r\n";
					if (write(s, header, strlen(header)) != strlen(header))
					{
						perror("Header write failed because ");
						exit(4);
					}

					const char *contentType = "Content-Type: text/html\r\n";
					if (write(s, contentType, strlen(contentType)) != strlen(contentType))
					{
						perror("ContentType write failed because ");
						exit(4);
					}

					fseek(file, 0, SEEK_END); 				// seek to end of file
					const int fileSize = ftell(file); 		// get current file pointer
					fseek(file, 0, SEEK_SET); 				// seek back to beginning of file
					char *fileContents = malloc(fileSize);

					char contentLength[100] = "Content-Length: ";
					char length[10];
					sprintf(length, "%d", fileSize);
					strcat(contentLength, length);
					strcat(contentLength, "\r\n\r\n");
					
					if (write(s, contentLength, strlen(contentLength)) != strlen(contentLength))
					{
						perror("ContentLength write failed because ");
						exit(4);
					}

					if (fileContents)
					{
						fread(fileContents, 1, fileSize, file);

						if (write(s, fileContents, strlen(fileContents)) != strlen(fileContents))
						{
							perror("HTML File write failed because ");
							exit(4);
						}
					}

					free(fileContents);
					fclose(file);
				}
				else
				{
					//error 404
					const char *error = "HTTP/1.1 404 Not Found\r\n";

					if (write(s, error, strlen(error)) != strlen(error))
					{
						perror("Error 404 write failed because ");
						exit(4);
					}
				}
			}
			else
			{
				FILE *file;
				char *fileName = malloc(100);

				if (strstr(token, "htm") != NULL)		//add html/ to input file name
				{
					strcat(fileName, "html/");
					strcat(fileName, token);
				} else
				if (strstr(token, "gif") != NULL)
				{
					strcpy(fileName, token);
				} else
					strcat(fileName, token);

				// strcat(fileName, "\0");
				printf("%ld\n", strlen(fileName));
				// printf("%s\n", fileName);

				if ((file = fopen(fileName, "r")) != NULL)
				{
					//write header
					const char *header = "HTTP/1.0 200 OK\r\n";
					if (write(s, header, strlen(header)) != strlen(header))
					{
						perror("Header write failed because ");
						exit(4);
					}

					if (strstr(token, "htm") != NULL)		//browser wants a .html file
					{
						const char *contentType = "Content-Type: text/html\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							exit(4);
						}
					} else
					if (strstr(token, "gif") != NULL)		//browser wants a .gif file
					{
						const char *contentType = "Content-Type: image/gif\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							exit(4);
						}
					} else
					if (strstr(token, "psd") != NULL)		//browser wants a .psd file
					{
						const char *contentType = "Content-Type: image/psd\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							exit(4);
						}
					} else
					if (strstr(token, "jpg") != NULL)		//browser wants a .jpg file
					{
						const char *contentType = "Content-Type: image/jpg\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							exit(4);
						}
					}

					fseek(file, 0, SEEK_END); 				// seek to end of file
					const int fileSize = ftell(file); 		// get current file pointer
					fseek(file, 0, SEEK_SET); 				// seek back to beginning of file
					char *fileContents = malloc(fileSize);

					char contentLength[100] = "Content-Length: ";
					char length[10];
					sprintf(length, "%d", fileSize);
					strcat(contentLength, length);
					strcat(contentLength, "\r\n\r\n");

					if (write(s, contentLength, strlen(contentLength)) != strlen(contentLength))
					{
						perror("ContentLength write failed because ");
						exit(4);
					}

					if (fileContents)
					{
						fread(fileContents, 1, fileSize, file);

						if (write(s, fileContents, strlen(fileContents)) != strlen(fileContents))
						{
							perror("Requested File write failed because ");
							exit(4);
						}
					}

					free(fileContents);
					fclose(file);
				}
				else
				{
					//error 404
					const char *error = "HTTP/1.1 404 Not Found\r\n";

					if (write(s, error, strlen(error)) != strlen(error))
					{
						perror("Error 404 write failed because ");
						exit(4);
					}
				}

				free(fileName);
			}
		}
	}
}