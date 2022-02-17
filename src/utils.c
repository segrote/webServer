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