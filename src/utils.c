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
	{
        perror("Can't open the file because ");
		fflush(stderr);
	}

    int ret = write(fd, line, strlen(line));
    if (ret < 0)
	{
        perror("Can't write to the file because ");
		fflush(stderr);
	}
}

void readConfigFile(char *inputFile, int *connections, char *port)
{
	FILE *configFile = fopen(inputFile, "r");
	if (configFile == NULL)		//failed to open file
	{
		perror("Couldn't open configFile ");
		fflush(stderr);
		exit(1);
	}

	size_t len = 100;
	char *line = (char *)malloc(sizeof(char) * len);

	if (getLineFromFile(configFile, line, len) != -1)		//read # connections
	{
		sscanf(line, "%d", *&connections);
	} else
	{
		perror("Can't get line from config file because ");
		fflush(stderr);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read rootDirectory
	{
		int length = strlen(line);
		if (length > 0 && line[length-1] == '\n')
			line[length-1] = '/';

		strcpy(rootDirectory, line);
	} else
	{
		perror("Can't get line from config file because ");
		fflush(stderr);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read indexFileName
	{
		int length = strlen(line);
		if (length > 0 && line[length-1] == '\n')
			line[length-1] = '\0';

		strcpy(indexFileName, line);
	} else
	{
		perror("Can't get line from config file because ");
		fflush(stderr);
	}

	if (getLineFromFile(configFile, line, len) != -1)		//read port number
	{
		strcpy(port, line);
	} else
	{
		perror("Can't get line from config file because ");
		fflush(stderr);
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
	{
		perror("Can't get service entry because ");
		fflush(stderr);
	}

    /* Map protocol name to protocol number */
	if ( (ppe = getprotobyname(transport)) == 0)
	{
		perror("Can't get protocol entry because ");
		fflush(stderr);
	}

    /* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

    /* Allocate a socket */
	s = socket(PF_INET, type, ppe->p_proto);
	if (s < 0)
	{
		perror("Can't create socket because ");
		fflush(stderr);
	}

	/* set SO_REUSEADDR on a socket to true (1): */
	int optval = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("Can't set sockopt because ");
		fflush(stderr);
	}

    /* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		perror("Can't bind to port because ");
		fflush(stderr);
	}

	if ((listen(s, connections)) != 0)
    {
        perror("Listen failed because ");
        fflush(stderr);
    }

	return s;
}

void accessLog(char *header)
{
	//decrypt header to common log format

	printf("%s", header);		//I got busy :(
}

void *readHandler(void *arg)
{
	int s = *(int*) arg;
	ssize_t ssize;
	char buffer[8190];
	char *rest;

	if ((ssize = read(s, &buffer, sizeof(buffer))) > 0)		//if we can read data from the client
	{
		accessLog(buffer);
		char *token = strtok_r(buffer, " ", &rest);

		if (strcmp(token, "GET") == 0)		//GET request received
		{
			token = strtok_r(NULL, " ", &rest);		//get filename

			if (strstr(token, "cgi") != NULL)		//get CGI query
			{
				// char *tok = malloc(strlen(token));
				// char *name = malloc(strlen(tok) - 9);
				// strcpy(tok, token);
				// strncpy(name, &tok[8], strlen(name));
				// printf("%s\n", tok);
				//string after 18th character is our requested file

				// if (setenv("QUERY_STRING", newQueryString, 1) == -1)
				// {
				// 	perror("setenv failed because ");
				// 	fflush(stderr);
				// }

				// free(tok);
				// free(name);

				// close(1);		//close stdout
				// dup2(s, 1);		//copy client to stdout

				// if (fork() == 0)	//child process
				// {
				// 	if (execv("/bin/sh", "cgi-bin/getFile.sh") == -1)	// test for execv fail
				// 	{
				// 		//error 500
				// 		const char *error = "HTTP/1.1 500 Internal Server Error\r\n";

				// 		if (write(s, error, strlen(error)) != strlen(error))
				// 		{
				// 			perror("Error 500 write failed because ");
				// 			fflush(stderr);
				// 			exit(4);
				// 		}
				// 	} else		//write header
				// 	{
				// 		printf("Content-Type: text/html\r\n\r\n");
				// 	}
				// }
			}

			if (strcmp(token, "/") == 0)	//get home.htm or default
			{
				FILE *file;

				if ((file = fopen(indexFileName, "r")) != NULL)
				{
					//write header
					const char *header = "HTTP/1.0 200 OK\r\n";
					if (write(s, header, strlen(header)) != strlen(header))
					{
						perror("Header write failed because ");
						fflush(stderr);
						exit(4);
					}

					const char *contentType = "Content-Type: text/html\r\n";
					if (write(s, contentType, strlen(contentType)) != strlen(contentType))
					{
						perror("ContentType write failed because ");
						fflush(stderr);
						exit(4);
					}

					if (fseek(file, 0, SEEK_END) == -1) 				// seek to end of file
					{
						perror("Requested file fseek failed because ");
						fflush(stderr);
					}

					const int fileSize = ftell(file); 		// get current file pointer
					if (fileSize == -1)
					{
						perror("Requested file ftell failed because ");
						fflush(stderr);
					}

					if (fseek(file, 0, SEEK_SET) == -1) 				// seek back to beginning of file
					{
						perror("Requested file fseek failed because ");
						fflush(stderr);
					}

					char *fileContents = malloc(fileSize); bzero(fileContents, fileSize);
					char contentLength[200]; bzero(contentLength, 200);
					strcat(contentLength, "Content-Length: ");
					char length[100]; bzero(length, 100);
					sprintf(length, "%d", fileSize);
					strcat(contentLength, length);
					strcat(contentLength, "\r\n");
					
					if (write(s, contentLength, strlen(contentLength)) != strlen(contentLength))
					{
						perror("ContentLength write failed because ");
						fflush(stderr);
						exit(4);
					}

					const char *connectionStatus = "Connection: close\r\n\r\n";
					if (write(s, connectionStatus, strlen(connectionStatus)) != strlen(connectionStatus))
					{
						perror("connectionStatus write failed because ");
						fflush(stderr);
						exit(4);
					}

					if (fileContents)
					{
						fread(fileContents, 1, fileSize, file);

						if (write(s, fileContents, fileSize) != fileSize)
						{
							perror("HTML File write failed because ");
							fflush(stderr);
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
						fflush(stderr);
						exit(4);
					}
				}
			}
			else
			{
				FILE *requestedFile;
				char *fileName = malloc(200); bzero(fileName, 200);
				char readFlag[5] = "r";

				if (strstr(token, "htm") != NULL)		//add rootDirectory/ to input file name
				{
					char *temp = malloc(strlen(token));
					strcat(temp, token);
					memmove(temp, temp+1, strlen(temp));
					temp[strlen(temp)] = '\0';
					strcat(fileName, rootDirectory);
					strcat(fileName, temp);
					free(temp);
				} else
				if (strstr(token, "gif") != NULL || strstr(token, "jpg") != NULL)
				{
					strcpy(readFlag, "rb");

					if (strstr(token, "images") == NULL)		//add images/ to input file name
					{
						char *tempVar = malloc(strlen(token));
						strcat(tempVar, token);
						memmove(fileName, fileName+1, strlen(fileName));
						tempVar[strlen(tempVar)] = '\0';
						strcat(fileName, "images/");
						strcat(fileName, tempVar);
						free(tempVar);
					}
					else
					{
						strcat(fileName, token);
						memmove(fileName, fileName+1, strlen(fileName));
						fileName[strlen(fileName)] = '\0';
					}
				}

				if ((requestedFile = fopen(fileName, readFlag)) != NULL)
				{
					//write header
					const char *header = "HTTP/1.0 200 OK\r\n";
					if (write(s, header, strlen(header)) != strlen(header))
					{
						perror("Header write failed because ");
						fflush(stderr);
						exit(4);
					}

					if (strstr(token, "htm") != NULL)		//browser wants a .html file
					{
						const char *contentType = "Content-Type: text/html\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							fflush(stderr);
							exit(4);
						}
					} else
					if (strstr(token, "gif") != NULL)		//browser wants a .gif file
					{
						const char *contentType = "Content-Type: image/gif\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							fflush(stderr);
							exit(4);
						}
					} else
					if (strstr(token, "jpg") != NULL)		//browser wants a .jpg file
					{
						const char *contentType = "Content-Type: image/jpeg\r\n";
						if (write(s, contentType, strlen(contentType)) != strlen(contentType))
						{
							perror("ContentType write failed because ");
							fflush(stderr);
							exit(4);
						}
					}

					if (fseek(requestedFile, 0, SEEK_END) == -1) 				// seek to end of file
					{
						perror("Requested file fseek failed because ");
						fflush(stderr);
					}

					const int fileSize = ftell(requestedFile); 		// get current file pointer
					if (fileSize == -1)
					{
						perror("Requested file ftell failed because ");
						fflush(stderr);
					}

					if (fseek(requestedFile, 0, SEEK_SET) == -1) 				// seek back to beginning of file
					{
						perror("Requested file fseek failed because ");
						fflush(stderr);
					}

					char *fileCon = malloc(fileSize); bzero(fileCon, fileSize);
					char contentLength[400]; bzero(contentLength, 400);
					strcat(contentLength, "Content-Length: ");
					char length[350]; bzero(length, 350);
					sprintf(length, "%d", fileSize);
					strcat(contentLength, length);
					strcat(contentLength, "\r\n");

					if (write(s, contentLength, strlen(contentLength)) != strlen(contentLength))
					{
						perror("ContentLength write failed because ");
						fflush(stderr);
						exit(4);
					}

					const char *connectionStatus = "Connection: close\r\n\r\n";
					if (write(s, connectionStatus, strlen(connectionStatus)) != strlen(connectionStatus))
					{
						perror("connectionStatus write failed because ");
						fflush(stderr);
						exit(4);
					}

					if (fileCon > 0)
					{
						fread(fileCon, 1, fileSize, requestedFile);

						if (write(s, fileCon, fileSize) != fileSize)
						{
							perror("Requested File write failed because ");
							fflush(stderr);
							exit(4);
						}
					}

					free(fileCon);
					fclose(requestedFile);
				}
				else
				{
					//error 404
					const char *error = "HTTP/1.1 404 Not Found\r\n";

					if (write(s, error, strlen(error)) != strlen(error))
					{
						perror("Error 404 write failed because ");
						fflush(stderr);
						exit(4);
					}
				}

				free(fileName);
			}
		}
		else
		if (strcmp(token, "POST") == 0)
		{
			token = strtok_r(NULL, " ", &rest);		//get filename
			// token should have "/POST.cgi"

			
		}
	} else
	{
		perror("Can't read data from the client because ");
		fflush(stderr);
	}
}