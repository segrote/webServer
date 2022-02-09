#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>

int main () {

	int s;
	int b;

	struct sockaddr_in sin;
	struct hostent *server;
	int addrlen, new_socket;
	struct sockaddr_in address;
	char buffer[512];
	ssize_t ssize;

	bzero((char *) &sin, sizeof(sin));

    	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(6000);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Could not create socket because ");
	}

	b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
	if (b < 0) {
		perror("Could not bind to socket because ");
	}

	listen(s, 12);

	addrlen = sizeof(struct sockaddr_in);
	new_socket = accept(s, (struct sockaddr *)&address, &addrlen);
	if (new_socket<0)
	        perror("Accept connection");
	else
		fprintf(stderr, "got a connection\n");

	ssize = recv(new_socket, &buffer, sizeof(buffer), 0);

	// from browser
	// GET /index.html Http/1.1
	// Con/........


	// hard code response:
	// HTTP/1.1 200 OK
	// Date: Sat, 28 Nov 2009 04:36:25 GMT
	// Server: LiteSpeed
	// Connection: close

	// 2 <CR>
	// file = open("index.html", ....);
	// read(file, buf, sizeof(buf));
	// write(socket-from-accept, buf, sizeof(buf));
	// close
	// https://code.tutsplus.com/tutorials/http-headers-for-dummies--net-8039
	if (ssize < 0)
		perror("Did not receive anything because ");
	else {
		fprintf(stderr,"Got \"%s\" from socket\n", buffer);
		fprintf(stderr,"String length = %d \n", strlen(buffer));
	}
}