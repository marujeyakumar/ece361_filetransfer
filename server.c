/*
** server.c -- a stream socket server demo
Referenced from http://beej.us/guide/bgnet/examples/client.c 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10 // how many pending connections queue will hold
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main (int argc, char *argv[]) {
	
	if (argc < 2) { 
		printf("too few args\n");
		exit(1);
	}

 	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
 	struct addrinfo hints, *servinfo, *p;
 	struct sockaddr_storage their_addr; // connector's address information
 	socklen_t sin_size;

 	int yes=1;
 	char s[INET6_ADDRSTRLEN];
 	int rv;

 	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_UNSPEC;
 	hints.ai_socktype = SOCK_STREAM;
 	hints.ai_flags = AI_PASSIVE; // use my IP
 	if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
 		return 1;
 	}

	 // loop through all the results and bind to the first we can
	 for(p = servinfo; p != NULL; p = p->ai_next) {
	 	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
	 		perror("server: socket");
	 		continue;
		 }
	 	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	 		close(sockfd);
	 		perror("server: bind");
	 		continue;
	 	}
	 	break;
	}

 	freeaddrinfo(servinfo); // all done with this structure

 	if (p == NULL) {
		 fprintf(stderr, "server: failed to bind\n");
		 exit(1);
	 }
 	if (listen(sockfd, BACKLOG) == -1) {
 		perror("listen");
 		exit(1);
	 }

 	printf("server: waiting for connections...\n");
 	while(1) { // main accept() loop
 		sin_size = sizeof their_addr;
 		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
 		if (new_fd == -1) {
 			perror("accept");
 			continue;
 		}
 		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
 		printf("server: got connection from %s\n", s);
 		
		//if (!fork()) { // this is the child process
 			//close(sockfd); // child doesn't need the listener

			char buffer[256] = {'a'};
			int fromlen = sizeof (struct sockaddr_storage); 

			if (recvfrom(new_fd, buffer, 256, 0, (struct sockaddr *)&their_addr, &sin_size) == -1) {
				perror("recvfrom");	
				exit(1);	
			}

		
			else if(buffer[0] == 'f' && buffer[1] == 't' && buffer[2]=='p') {
				printf("server: message received \"%s\"\n", buffer);			
				int numbytes=5;
				if ( (numbytes= send(new_fd, "yes", 3, 0)) == -1) {
					perror("sendto yes");
					exit(1);	
				} else {
					printf("sent %d bytes\n", numbytes );
					break;
				}
			} else {
				printf("server: message received \"%s\"\n", buffer);			
				if ( send(new_fd, "no", 2, 0) == -1) {
					perror("sendto no");
					exit(1);
				}
			} 
			
			printf("closing new_fd\n"); 			
			close(new_fd);
 			//exit(0);
 		//}
	// close(new_fd); // parent doesn't need this
	 }
 close(sockfd);
 return 0;
} 
	

