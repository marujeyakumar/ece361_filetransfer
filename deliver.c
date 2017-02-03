/*
** client.c -- a stream socket client demo
Referenced from http://beej.us/guide/bgnet/examples/client.c 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int file_exists(char* fname) {
	if( access( fname, F_OK ) != -1 ) {
    // file exists
		return 1;
	} else {
    // file doesn't exist
		return 0;
	}
}

int main(int argc, char *argv[]) {
	
	char file_name[256];
	printf("Input message: ftp <file name>: ");
	scanf("%s", file_name);

	int exists = file_exists(file_name);
	
	//printf("file name is %s\n", file_name);
	printf("file exists %d (0:doesn't exists, 1:exists)\n", exists);
	
	//exit if file does not exist
	if (exists == 0) { 
		return 0;
	}


	int sockfd, numbytes;
 	char buf[256];
	struct addrinfo hints, *servinfo, *p;
 	int rv;
 	char s[INET6_ADDRSTRLEN];
 	
	if (argc != 3) {
 		fprintf(stderr,"usage: server_addr server_portnumber\n");
 		exit(1);
 	}

 	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_UNSPEC;
 	hints.ai_socktype = SOCK_STREAM;
 	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
 		return 1;
 	}

 	// loop through all the results and connect to the first we can
 	for(p = servinfo; p != NULL; p = p->ai_next) {
 		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
 			printf("client: socket");
			continue;
		}
 		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
 			close(sockfd);
 			printf("client: connect");
 			continue;
 		}
 		break;
 	}

 	if (p == NULL) {
 		printf("client: failed to connect\n");
 		return 2;
 	} 
 	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
 	printf("client: connecting to %s\n", s);
 	freeaddrinfo(servinfo); // all done with this structure
 	
	if (send( sockfd, "ftp", 3, 0 ) == -1) {
		printf("client: sendto fail\n");	
	};

	
	if ((numbytes = recv(sockfd, buf, 4, 0)) == -1) {
 		perror("recv");
 		exit(1);
 	} 
 
	buf[numbytes] = '\0';
 	printf("client: received '%s which is %d bytes\n", buf, strlen(buf));

	int i = 0;
	if(buf[0] == 'y' && buf[1] == 'e' && buf[2] == 's') {
		printf("A file transfer can start\n");	
	} else {
		exit(1);	
	}

 	close(sockfd);
 	return 0;
}

